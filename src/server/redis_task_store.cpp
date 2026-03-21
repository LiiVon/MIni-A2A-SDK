#include "../../include/server/redis_task_store.hpp"
#include <nlohmann/json.hpp>

namespace a2a
{

RedisTaskStore::RedisTaskStore(const std::string& redis_url)
{
    redis_ = std::make_shared<sw::redis::Redis>(redis_url);
}

std::string RedisTaskStore::task_key(const std::string& task_id) const
{
    return "a2a:task:" + task_id;
}

std::string RedisTaskStore::history_key(const std::string& task_id) const
{
    return "a2a:history:" + task_id;
}

std::string RedisTaskStore::artifact_key(const std::string& task_id) const
{
    return "a2a:artifact:" + task_id;
}

std::optional<AgentTask> RedisTaskStore::get_task(const std::string& task_id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto key = task_key(task_id);
    auto task_json = redis_->get(key);

    if(!task_json)
        return std::nullopt;

    return AgentTask::from_json(nlohmann::json::parse(*task_json));
}

void RedisTaskStore::set_task(const AgentTask& task)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto key = task_key(task.id());
    // 序列化整个任务（包含status/artifact/history等初始部分）
    redis_->set(key, task.to_json());
}

void RedisTaskStore::update_status(const std::string& task_id, TaskState status, const std::string& message)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto opt_task = get_task(task_id);
    if (!opt_task) return;

    AgentTask task = *opt_task;
    AgentTaskStatus new_status(status);
    if (!message.empty())
        new_status.set_message(message);
    task.set_status(new_status);

    set_task(task); // 重新覆盖
}

void RedisTaskStore::add_artifact(const std::string& task_id, const Artifact& artifact)
{
    std::lock_guard<std::mutex> lock(mutex_);
    // 1. 历史方式：直接push到 artifact list
    auto key = artifact_key(task_id);
    redis_->rpush(key, artifact.to_json());

    // 2. 更新task本身（冗余安全，可删）
    auto opt_task = get_task(task_id);
    if(!opt_task) return;
    AgentTask task = *opt_task;
    task.add_artifact(artifact);
    set_task(task);
}

void RedisTaskStore::add_history_message(const std::string& task_id,const AgentMessage& message)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto key = history_key(task_id);
    redis_->rpush(key, message.to_json());

    // 也可冗余更新 task 对象本身
    auto opt_task = get_task(task_id);
    if(!opt_task) return;
    AgentTask task = *opt_task;
    task.add_history_message(message);
    set_task(task);
}

std::vector<AgentMessage> RedisTaskStore::get_history(const std::string& task_id, int max_length)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto key = history_key(task_id);

    long redis_len = redis_->llen(key);
    long start = 0;
    long end = redis_len - 1;

    if(max_length > 0 && max_length < redis_len)
        start = redis_len - max_length;

    std::vector<std::string> msgs_json;
    redis_->lrange(key, start, end, std::back_inserter(msgs_json));

    std::vector<AgentMessage> msgs;
    for(const auto& j : msgs_json)
    {
        msgs.push_back(AgentMessage::from_json(nlohmann::json::parse(j)));
    }
    return msgs;
}

bool RedisTaskStore::delete_task(const std::string& task_id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto tkey = task_key(task_id);
    auto hkey = history_key(task_id);
    auto akey = artifact_key(task_id);

    long n = redis_->del({tkey, hkey, akey});
    return n > 0;
}

bool RedisTaskStore::task_exists(const std::string& task_id)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto key = task_key(task_id);
    return redis_->exists(key);
}

size_t RedisTaskStore::size() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    return 0;
}

void RedisTaskStore::clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    // WARNING: demo，不建议杀全库
    redis_->flushdb();
}

}
