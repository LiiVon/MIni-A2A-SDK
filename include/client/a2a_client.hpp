#pragma once

#include "../models/agent_message.hpp"
#include "../models/agent_task.hpp"
#include "../models/message_send_params.hpp"
#include "../core/http_client.hpp"
#include "../models/a2a_response.hpp"
#include <string>
#include <optional>
#include <functional>

namespace a2a 
{
    class A2AClient 
    {
    public:
    explicit A2AClient(const std::string& base_url);
    ~A2AClient();

    A2AClient(const A2AClient&) = delete;
    A2AClient& operator=(const A2AClient&) = delete;
    A2AClient(A2AClient&&) noexcept;
    A2AClient& operator=(A2AClient&&) noexcept;

    A2AResponse send_message(const MessageSendParams& params);

    void send_message_streaming(const MessageSendParams& params,
            std::function<void(const std::string&)> callback);

    AgentTask get_task(const std::string& task_id);
    AgentTask cancel_task(const std::string& task_id);

    void subscribe_to_task(const std::string& task_id,
                std::function<void(const std::string&)> callback);
    void set_timeout(long timeout);
    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };


} // namespace a2a