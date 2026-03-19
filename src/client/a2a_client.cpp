#include "../../include/client/a2a_client.hpp"
#include "../../include/core/a2a_methods.hpp"
#include "../../include/core/exception.hpp"
#include "../../include/core/jsonrpc_request.hpp"
#include "../../include/core/jsonrpc_response.hpp"
#include "../../include/models/agent_message.hpp"
#include <nlohmann/json.hpp>
#include <sstream>
#include <ctime>

using json = nlohmann::json;

namespace a2a
{
    //  生成请求唯一 ID
    static std::string generate_uuid()
    {
        static int cnt = 0;
        std::ostringstream oss;
        oss <<"req-" << ++cnt <<"-"<<std::time(nullptr);
        return oss.str();
    }

    // pimpl
    struct A2AClient::Impl 
    {
        explicit Impl(const std::string& base_url)
            : base_url_(base_url)
            , http_client_()
        {
            if(!base_url_.empty() && base_url_.back() == '/')
            {
                base_url_.pop_back();
            }
        }
        std::string base_url_;
        HttpClient http_client_;

        JsonRpcResponse send_rpc_request(const std::string& method,
                    const std::string& params_json)
        {
            JsonRpcRequest request(generate_uuid(), method, params_json);
            std::string request_json = request.to_json();
            
            // 发送 HTTP POST 请求，得到响应
            auto http_response = http_client_.post(
                base_url_,
                request_json,
                "application/json"
            );

            // 网络层报错
            if(!http_response.is_success())
            {
                throw A2AException(
                    "HTTP request failed with status code: " + std::to_string(http_response.status_code),
                    ErrorCode::InternalError
                );
            }

            JsonRpcResponse rpc_response = JsonRpcResponse::from_json(http_response.body);  
            // 业务层报错
            if(rpc_response.is_error())
            {
                const auto& error = rpc_response.error();
                throw A2AException(
                    std::string("RPC request failed with error: ") + error->message,
                    ErrorCode::InternalError
                );
            }
            return rpc_response;
        }
    };

    A2AClient::A2AClient(const std::string& base_url)
        : impl_(std::make_unique<Impl>(base_url)) {}
    
    A2AClient::~A2AClient() = default;
    A2AClient::A2AClient(A2AClient&&) noexcept = default;
    A2AClient& A2AClient::operator=(A2AClient&&) noexcept = default;

    A2AResponse A2AClient::send_message(const MessageSendParams& parans)
    {
        std::string params_json = parans.to_json();
        auto rpc_response = impl_->send_rpc_request("sendMessage", params_json);
        
        if(!rpc_response.result_json().has_value())
        {
            throw A2AException(
                "RPC response missing result field",
                ErrorCode::InternalError
            );
        }

        const std::string& reseult_json_str = *rpc_response.result_json();

        try
        {
            json j = json::parse(reseult_json_str);
            if(j.contains("status"))
            {
                return A2AResponse(AgentTask::from_json(reseult_json_str));
            }
            else
            {
                return A2AResponse(AgentMessage::from_json(reseult_json_str));
            }
        }
        catch(const std::exception& e)
        {
            throw A2AException(
                "Failed to parse RPC response: " + std::string(e.what()),
                ErrorCode::InternalError
            );
        }
    }

    void A2AClient::send_message_streaming(const MessageSendParams& params, std::function<void(const std::string&)> callback)
    {
        std::string params_json = params.to_json();
        JsonRpcRequest request(generate_uuid(), "sendMessageStreaming", params_json);

        impl_->http_client_.post_stream(
            impl_->base_url_,
            request.to_json(),
            "application/json",
                callback
        );
    }

    AgentTask A2AClient::get_task(const std::string& task_id)
    {
        TaskIdParams params;
        params.id = task_id;

        auto rpc_response = impl_->send_rpc_request("getTask", params.to_json());

        if(!rpc_response.result_json().has_value())
        {
            throw A2AException(
                "RPC response missing result field",
                ErrorCode::InternalError
            );
        }
        return AgentTask::from_json(*rpc_response.result_json());
    }

    AgentTask A2AClient::cancel_task(const std::string& task_id) 
    {
        TaskIdParams params;
        params.id = task_id;
    
        auto response = impl_->send_rpc_request(A2AMethods::TASK_CANCEL, params.to_json());
    
        if (!response.result_json().has_value()) 
        {
            throw A2AException("No result in response", ErrorCode::InternalError);
        }
        return AgentTask::from_json(*response.result_json());
    }

    void A2AClient::subscribe_to_task(const std::string& task_id,std::function<void(const std::string&)> callback) 
    {
        TaskIdParams params;
        params.id = task_id;
        JsonRpcRequest request(generate_uuid(), A2AMethods::TASK_SUBSCRIBE, params.to_json());
        
        impl_->http_client_.post_stream(
            impl_->base_url_,
            request.to_json(),
            "application/json",
            callback
        );
    }

    void A2AClient::set_timeout(long seconds) 
    {
        impl_->http_client_.set_timeout(seconds);
    }
}