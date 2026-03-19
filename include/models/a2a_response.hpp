#pragma once

#include "agent_task.hpp"
#include "agent_message.hpp"

namespace a2a
{
    class A2AResponse
    {
    public:
        enum class Type
        {
            Task,
            Message
        };

        A2AResponse(const AgentTask& task)
            : type_(Type::Task)
            , task_(task)
            ,message_() 
            {}
    
        A2AResponse(const AgentMessage& message)
            : type_(Type::Message)
            , task_()
            , message_(message)
            {}

        Type type() const { return type_; }
        const AgentTask& as_task() const { return task_; }
        const AgentMessage& as_message() const { return message_; }

        bool is_task() const { return type_ == Type::Task; }
        bool is_message() const { return type_ == Type::Message; }

    private:
        Type type_;
        AgentTask task_;
        AgentMessage message_;
    };
}