#include "../../include/server/task_manager.hpp"
#include "../../include/server/memory_task_store.hpp"
#include "../../include/core/exception.hpp"
#include <sstream>

namespace a2a
{
    static std::string generate_task_id()
    {
        static int cnt =0;
        std::ostringstream oss;
        oss << "task-" << std::time(nullptr) << "-" << cnt++;
        return oss.str();
    }

    static std::string generate_context_id()
    {
        static int cnt =0;
        std::ostringstream oss;
        oss << "context-" << std::time(nullptr) << "-" << cnt++;
        return oss.str();
    }

    struct TaskManager::Impl
    {
        explicit Impl(std::shared_ptr<ITaskStore> task_store)
            :task_store_(task_store ? task_store: std::make_shared<MemoryTaskStore>())
            ,on_message_received_()
            ,on_task_created_()
            ,on_task_updated_()
            ,on_task_cancelled_()
            ,on_agent_card_query_()
            {}
        std::shared_ptr<ITaskStore> task_store_;
        MessageCallback on_message_received_;
        TaskCallback on_task_created_;
        TaskCallback on_task_updated_;
        TaskCallback on_task_cancelled_;
        AgentCardCallback on_agent_card_query_;
    };

    TaskManager::TaskManager(std::shared_ptr<ITaskStore> task_store)
        :impl_(std::make_unique<Impl>(task_store)) {} 

    TaskManager::TaskManager(TaskManager&&) noexcept = default;
    TaskManager& TaskManager::operator=(TaskManager&&) noexcept = default;

    void TaskManager::set_on_message_received(MessageCallback callback)
    {
        impl_->on_message_received_ = std::move(callback);
    }

    void TaskManager::set_on_task_created(TaskCallback callback)
    {
        impl_->on_task_created_ = std::move(callback);
    }

    void TaskManager::set_on_task_updated(TaskCallback callback)
    {
        impl_->on_task_updated_ = std::move(callback);
    }

    void TaskManager::set_on_task_cancelled(TaskCallback callback)
    {
        impl_->on_task_cancelled_ = std::move(callback);
    }

    void TaskManager::set_agent_card_query(AgentCardCallback callback)
    {
        impl_->on_agent_card_query_ = std::move(callback);
    }

    AgentTask TaskManager::create_task(const std::string& context_id,
                const std::string& task_id)
    {
        std::string actual_ocntext_id = context_id.empty() ? generate_context_id() : context_id;
        std::string actual_task_id = task_id.empty() ? generate_task_id() : task_id;
        
        AgentTask task(actual_task_id, actual_ocntext_id);
        task.set_status(TaskState::Submitted);

        impl_->task_store_->set_task(task);

        if(impl_->on_task_created_)
        {
            impl_->on_task_created_(task);
        }

        return task;
    }

    AgentTask TaskManager::get_task(const std::string& task_id)
    {
        auto task_opt = impl_->task_store_->get_task(task_id);
        if(!task_opt)
        {
            throw A2AException("Task not found: " + task_id, ErrorCode:: TaskNotFound);
        }
        return *task_opt;
    }

    AgentTask TaskManager::cancel_task(const std::string& task_id)
    {
        auto task_opt = impl_->task_store_->get_task(task_id);
        if(!task_opt)
        {
            throw A2AException("Task not found: " + task_id, ErrorCode:: TaskNotFound);
        }

        AgentTask task = *task_opt;
        if(task.is_terminal())
        {
            throw A2AException("Task is already in terminal state and cannot be cancelled: " + task_id, ErrorCode:: TaskNotCancelable);
        }

        task.set_status(TaskState::Canceled);
        impl_->task_store_->set_task(task);

        if(impl_->on_task_cancelled_)
        {
            impl_->on_task_cancelled_(task);
        }

        return task;
    }

    void TaskManager::update_status(const std::string& task_id,TaskState status,const AgentMessage* message) 
    {
        std::string msg_text;

        if (message) 
        {
            impl_->task_store_->add_history_message(task_id, *message);
        }
        
        impl_->task_store_->update_status(task_id, status, msg_text);
        
        auto task_opt = impl_->task_store_->get_task(task_id);
        if (task_opt.has_value() && impl_->on_task_updated_) 
        {
            impl_->on_task_updated_(*task_opt);
        }
    }

    void TaskManager::return_artifact(const std::string& task_id,const Artifact& artifact) 
    {
        impl_->task_store_->add_artifact(task_id, artifact);
        
        auto task_opt = impl_->task_store_->get_task(task_id);
        if (task_opt.has_value() && impl_->on_task_updated_) 
        {
            impl_->on_task_updated_(*task_opt);
        }
    }

    A2AResponse TaskManager::send_message(const MessageSendParams& params) 
    {
        if (!impl_->on_message_received_) 
        {
            throw A2AException(
                "OnMessageReceived callback not set",
                ErrorCode::InternalError
            );
        }
        
        if (params.message().task_id().has_value()) 
        {
   
            const std::string& task_id = *params.message().task_id();
            
            auto task_opt = impl_->task_store_->get_task(task_id);
            if (!task_opt.has_value()) 
            {
                throw A2AException("Task not found: " + task_id, ErrorCode::TaskNotFound);
            }
            
            impl_->task_store_->add_history_message(task_id, params.message());
        }

        return impl_->on_message_received_(params);
    }

    void TaskManager::send_message_streaming(const MessageSendParams& params,std::function<void(const std::string&)> callback) 
    {
        if (!impl_->on_message_received_) 
        {
            throw A2AException(
                "OnMessageReceived callback not set",
                ErrorCode::InternalError
            );
        }

        
        auto response = impl_->on_message_received_(params);
        
        if (response.is_task())
         {
            callback(response.as_task().to_json());
        } else {
            callback(response.as_message().to_json());
        }
    }

    AgentCard TaskManager::get_agent_card(const std::string& agent_url) 
    {
        if (!impl_->on_agent_card_query_) 
        {
            return AgentCard::create()
                .with_name("Unknown Agent")
                .with_description("No description available")
                .with_url(agent_url)
                .with_version("1.0.0");
        }
        
        return impl_->on_agent_card_query_(agent_url);
    }

    std::shared_ptr<ITaskStore> TaskManager::get_task_store() const 
    {
        return impl_->task_store_;
    }
}