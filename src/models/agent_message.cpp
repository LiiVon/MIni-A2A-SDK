#include "../../include/models/agent_message.hpp"
#include <nlohmann/json.hpp>
#include "../../include/core/exception.hpp"

using json = nlohmann::json;

namespace a2a 
{

std::string AgentMessage::to_json() const 
{
    json j;
    
    j["messageId"] = message_id_;
    j["role"] = to_string(role_); 
    
    if (context_id_.has_value()) 
    {
        j["contextId"] = *context_id_;
    }
    
    if (task_id_.has_value()) 
    {
        j["taskId"] = *task_id_;
    }

    json parts_array = json::array();
    for (const auto& part : parts_) 
    {
        if (part) 
        {
            //先用 json::parse() 把它变回 JSON 对象，再塞进数组
            parts_array.push_back(json::parse(part->to_json()));
        }
    }
    j["parts"] = parts_array;
    
    return j.dump();
}

AgentMessage AgentMessage::from_json(const std::string& json_str)
{
    try 
    {
       
        json j = json::parse(json_str);
        AgentMessage msg;
        msg.message_id_ = j.value("messageId", "");
        
        if (j.contains("role") && j["role"].is_string()) 
        {
            msg.role_ = message_role_from_string(j["role"].get<std::string>());
        }
        
        if (j.contains("contextId") && j["contextId"].is_string()) 
        {
            msg.context_id_ = j["contextId"].get<std::string>();
        }
        
        if (j.contains("taskId") && j["taskId"].is_string())
        {
            msg.task_id_ = j["taskId"].get<std::string>();
        }
        
        if (j.contains("parts") && j["parts"].is_array()) 
        {
            for (const auto& part_json : j["parts"]) 
            {
                auto part = Part::from_json(part_json.dump());
                if (part) 
                {
                    msg.parts_.push_back(std::move(part));
                }
            }
        }
        return msg;
        
    } 
    catch (const json::exception& e) 
    {
        throw A2AException(std::string("Failed to parse AgentMessage from JSON: ") + e.what(), 
                        ErrorCode::ParseError);
    }
}

} // namespace a2a