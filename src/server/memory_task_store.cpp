#include "../../include/server/memory_task_store.hpp"
#include <algorithm>

namespace a2a
{
    std::optional<AgentTask> MemoryTaskStore::get_task(const std::string& task_id)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = tasks_.find(task_id);
        if (it != tasks_.end()) 
        {
            return it->second;
        }
        return std::nullopt;
    }

    void MemoryTaskStore::set_task(const AgentTask& task)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        tasks_[task.id()] = task;
    }

    void MemoryTaskStore::update_status(const std::string& task_id, TaskState status,const std::string& message)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = tasks_.find(task_id);
        if (it != tasks_.end()) 
        {
            AgentTaskStatus new_status(status);
            if(!message.empty())
            {
                new_status.set_message(message);
            }
            it->second.set_status(new_status);
        }
    }

    void MemoryTaskStore::add_artifact(const std::string& task_id,const Artifact& artifact)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = tasks_.find(task_id);
        if (it != tasks_.end()) 
        {
            it->second.add_artifact(artifact);
        }
    }

    std::vector<AgentMessage> MemoryTaskStore::get_history(const std::string& task_id,int max_length)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = tasks_.find(task_id);
        if(it == tasks_.end())
        {
            return {};
        }
        const auto& history = it->second.history();
        if(max_length <= 0 || static_cast<size_t>(max_length) >= history.size())
        {
            return history;
        }
        return std::vector<AgentMessage>(history.end() - max_length, history.end());
    }

    bool MemoryTaskStore::delete_task(const std::string& task_id)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = tasks_.find(task_id);

        if(it != tasks_.end())
        {
            tasks_.erase(it);
            return true;
        }
        return false;
    }

    bool MemoryTaskStore::task_exists(const std::string& task_id)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return tasks_.find(task_id) != tasks_.end();
    }

    size_t MemoryTaskStore::size() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return tasks_.size();
    }

    void MemoryTaskStore::clear()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        tasks_.clear();
    }
}