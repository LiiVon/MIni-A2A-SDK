#pragma once

#include "agent_message.hpp"
#include <optional>

namespace a2a
{
    // 消灭了长参数列表，使用一个结构体来封装 message/send 方法的参数
    class MessageSendParams
    {
    public:
        MessageSendParams() = default;

        explicit MessageSendParams(const AgentMessage& message)
            : message_(message) {}
     
        const AgentMessage& message() const { return message_; }
        const std::optional<int>& history_length() const { return history_length_; }
        const std::optional<std::string>& context_id() const { return context_id_; }
        const std::optional<std::string>& task_id() const { return task_id_; }

        void set_message(const AgentMessage& message) { message_ = message; }
        void set_history_length(const std::optional<int>& length) { history_length_ = length ;}
        void set_context_id(const std::optional<std::string>& context_id) { context_id_ = context_id; }
        void set_task_id(const std::optional<std::string>& task_id) { task_id_ = task_id; }

  
        std::string to_json() const;
        static MessageSendParams from_json(const std::string& json_str);

        static MessageSendParams create(const AgentMessage& message)
        {
            return MessageSendParams(message);
        }

        MessageSendParams& with_message(const AgentMessage& message)
        {
            message_ = message;
            return *this;
        }

        MessageSendParams& with_history_length(int length)
        {
            history_length_ = length;
            return *this;
        }

        MessageSendParams& with_context_id(const std::string& context_id)
        {
            context_id_ = context_id;
            return *this;
        }

        MessageSendParams& with_task_id(const std::string& task_id)
        {
            task_id_ = task_id;
            return *this;
        }

        private:
            AgentMessage message_;
            std::optional<int> history_length_;
            std::optional<std::string> context_id_;
            std::optional<std::string> task_id_;

    };
    struct TaskQueryParams
    {
        std::string id;
        std::optional<int> history_length;
        std::map<std::string, std::string> metadata;

        std::string to_json() const;
        static TaskQueryParams from_json(const std::string& json_str);
    };

    struct TaskIdParams
    {
        std::string id;
        std::string to_json() const;
        static TaskIdParams from_json(const std::string& json_str);
    };
}