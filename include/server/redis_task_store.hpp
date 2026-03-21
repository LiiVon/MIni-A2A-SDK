#pragma once

#include "task_store.hpp"
#include <sw/redis++/redis++.h>     
#include <mutex>
#include <memory>
#include <string>
#include <optional>

namespace a2a
{
    class RedisTaskStore : public ITaskStore
    {
    public:
        // 传入 Redis 地址（可增强支持 user/pass、dbnum、cluster 等）
        explicit RedisTaskStore(const std::string& redis_url);
        ~RedisTaskStore() override = default;

        std::optional<AgentTask> get_task(const std::string& task_id) override;
        void set_task(const AgentTask& task) override;
        void update_status(const std::string& task_id, TaskState status, const std::string& message = "") override;
        void add_artifact(const std::string& task_id, const Artifact& artifact) override;
        void add_history_message(const std::string& task_id, const AgentMessage& message) override;
        std::vector<AgentMessage> get_history(const std::string& task_id, int max_length = 0) override;
        bool delete_task(const std::string& task_id) override;
        bool task_exists(const std::string& task_id) override;

        size_t size() const;
        void clear();

    private:
        // 用唯一的 Redis 连接，线程安全由 hiredis 保证
        std::shared_ptr<sw::redis::Redis> redis_;
        mutable std::mutex mutex_; // 防多线程操作

        // 工单存储采用 hash：任务ID -> JSON串
        std::string task_key(const std::string& task_id) const;
        // 消息历史采用 list
        std::string history_key(const std::string& task_id) const;
        // 产物采用 list
        std::string artifact_key(const std::string& task_id) const;
    };
}
