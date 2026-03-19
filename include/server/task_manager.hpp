#pragma once 

#include "task_store.hpp"
#include "../models/agent_task.hpp"
#include "../models/agent_card.hpp"
#include "../models/agent_message.hpp"
#include "../models/message_send_params.hpp"
#include "../models/a2a_response.hpp"
#include <functional>
#include <memory>
#include <string>

namespace a2a
{
    class TaskManager
    {
    public:
        using MessageCallback = std::function<A2AResponse(const MessageSendParams&)>;
        using TaskCallback = std::function<void(const AgentTask&)>;
        using AgentCardCallback = std::function<AgentCard(const std::string& agent_url)>;
        
        explicit TaskManager(std::shared_ptr<ITaskStore> task_store = nullptr);
        ~TaskManager() = default;

        TaskManager(const TaskManager&) = delete;
        TaskManager& operator=(const TaskManager&) = delete;
        TaskManager(TaskManager&&) noexcept;
        TaskManager& operator=(TaskManager&&) noexcept;

        void set_on_message_received(MessageCallback callback);
        void set_on_task_created(TaskCallback callback);
        void set_on_task_updated(TaskCallback callback);
        void set_on_task_cancelled(TaskCallback callback);

        void set_agent_card_query(AgentCardCallback callback);

        AgentTask create_task(const std::string& econtext_id = "", const std::string& task_id = "");
        AgentTask get_task(const std::string& task_id);
        AgentTask cancel_task(const std::string& task_id);
        
        void update_status(const std::string& task_id,TaskState status,const AgentMessage* message = nullptr);
        void return_artifact(const std::string& task_id,const Artifact& artifact);

        A2AResponse send_message(const MessageSendParams& params);
        void send_message_streaming(const MessageSendParams& parms,std::function<void(const std::string&)> callback);

        AgentCard get_agent_card(const std::string& agent_url);
        std::shared_ptr<ITaskStore> get_task_store() const;
    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };
}