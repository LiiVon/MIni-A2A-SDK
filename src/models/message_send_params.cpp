#include "../../include/models/message_send_params.hpp"
#include "../../include/core/exception.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace a2a 
{
    std::string MessageSendParams::to_json() const 
    {
        json j;
        // 嵌套的 Message 对象。注意要先 parse 变成 json 对象，防止被当成纯字符串
        j["message"] = json::parse(message_.to_json());
        
        // 选填字段：有值才放进去
        if (history_length_.has_value()) j["historyLength"] = *history_length_;
        if (context_id_.has_value()) j["contextId"] = *context_id_;
        if (task_id_.has_value()) j["taskId"] = *task_id_;
        
        return j.dump();
    }

    MessageSendParams MessageSendParams::from_json(const std::string& json_str) 
    {
        try 
        {
            json j = json::parse(json_str);
            MessageSendParams params;
            
            // 委托给 AgentMessage 自己去解析
            if (j.contains("message") && j["message"].is_object()) 
            {
                params.set_message(AgentMessage::from_json(j["message"].dump()));
            }
            
            // 安全提取数字
            if (j.contains("historyLength") && j["historyLength"].is_number()) 
            {
                params.set_history_length(j["historyLength"].get<int>());
            }
            
            // 安全提取字符串
            if (j.contains("contextId") && j["contextId"].is_string()) 
            {
                params.set_context_id(j["contextId"].get<std::string>());
            }
            
            if (j.contains("taskId") && j["taskId"].is_string()) {
                params.set_task_id(j["taskId"].get<std::string>());
            }
            
            return params;
        } 
        catch (const json::exception& e) 
        {
            throw A2AException(std::string("Failed to parse MessageSendParams from JSON: ") + std::string(e.what())
            , ErrorCode::ParseError);
        }
    }

    std::string TaskQueryParams::to_json() const
    {
        json j;
        
        j["id"] = id;
        
        if (history_length.has_value()) 
        {
            j["historyLength"] = *history_length;
        }

        if (!metadata.empty()) 
        {
            j["metadata"] = metadata;
        }
        
        return j.dump();
    }

    TaskQueryParams TaskQueryParams::from_json(const std::string& json_str) 
    {
        try 
        {
            json j = json::parse(json_str);
            TaskQueryParams params;
            
            params.id = j.value("id", "");
            
            if (j.contains("historyLength") && j["historyLength"].is_number()) 
            {
                params.history_length = j["historyLength"].get<int>();
            }
            
            if (j.contains("metadata") && j["metadata"].is_object()) 
            {
                params.metadata = j["metadata"].get<std::map<std::string, std::string>>();
            }
            
            return params;
        } 
        catch (const json::exception& e) 
        {
            throw A2AException(std::string("Failed to parse TaskQueryParams from JSON: ") + std::string(e.what())
            , ErrorCode::ParseError);
        }
    }



    std::string TaskIdParams::to_json() const 
    {
        json j;
        j["id"] = id;
        return j.dump();
    }

    TaskIdParams TaskIdParams::from_json(const std::string& json_str) 
    {
        try 
        {
            json j = json::parse(json_str);
            TaskIdParams params;
            
            params.id = j.value("id", "");
            
            return params;
        } 
        catch (const json::exception& e) 
        {
            throw A2AException(std::string("Failed to parse TaskIdParams from JSON: ") + std::string(e.what())
            , ErrorCode::ParseError);
        }
    }

} // namespace a2a