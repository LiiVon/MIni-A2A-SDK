#pragma once

#include "../../include/models/agent_task.hpp"
#include <string>
#include <memory>
#include <optional>

namespace a2a
{
    class ITaskStore
    {
    public:
    virtual ~ITaskStore() = default;
    
    virtual std::optional<AgentTask> get_task(const std::string& task_id) = 0;
    virtual void set_task(const AgentTask& task) = 0;
    
    virtual void update_status(const std::string& task_id, TaskState status,const std::string& message = "") = 0;
    virtual void add_artifact(const std::string& task_id,const Artifact& artifact) = 0;
    
    virtual void add_history_message(const std::string& task_id,const AgentMessage& message) = 0;
    virtual std::vector<AgentMessage> get_history(const std::string& task_id,int max_length =0 ) = 0;
    
    virtual bool delete_task(const std::string& task_id) = 0;
    virtual bool task_exists(const std::string& task_id) = 0;
    };
}