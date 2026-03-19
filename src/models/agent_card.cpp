#include "../../include/models/agent_card.hpp"
#include <nlohmann/json.hpp>
#include "../../include/core/exception.hpp"

using json = nlohmann::json;

namespace a2a
{
    std::string AgentCapabilities::to_json() const
    {
        json j;
        j["streaming"] = streaming;
        j["push_notification"] = push_notification;
        j["task_management"] = task_management;
        return j.dump();
    }

    AgentCapabilities AgentCapabilities::from_json(const std::string& json_str)
    {
        try 
        {
            json j = json::parse(json_str);
            AgentCapabilities caps;
            caps.streaming = j.value("streaming", false);
            caps.push_notification = j.value("push_notification", false);
            caps.task_management = j.value("task_management", true);
            return caps;
        } 
        catch (const json::exception&) 
        {
            throw A2AException("Failed to parse AgentCapabilities JSON", ErrorCode::ParseError);
        }
    }

    std::string AgentSkill::to_json() const
    {
        json j;
        j["name"] = name;
        j["description"] = description;
        if (!input_modes.empty()) j["inputModes"] = input_modes;
        if (!output_modes.empty()) j["outputModes"] = output_modes;
        
        return j.dump();
    }

    AgentSkill AgentSkill::from_json(const std::string& json_str)
    {
        try 
        {
            json j = json::parse(json_str);
            AgentSkill skill;
            
            skill.name = j.value("name", "");
            skill.description = j.value("description", "");
            
            if (j.contains("inputModes") && j["inputModes"].is_array()) 
            {
                skill.input_modes = j["inputModes"].get<std::vector<std::string>>();
            }
            if (j.contains("outputModes") && j["outputModes"].is_array()) 
            {
                skill.output_modes = j["outputModes"].get<std::vector<std::string>>();
            }
            
            return skill;
        } 
        catch (const json::exception&) 
        {
            throw A2AException("Failed to parse AgentSkill JSON", ErrorCode::ParseError);
        }
    }

    std::string AgentProvider::to_json() const
    {
        json j;
        j["name"] = name;
        j["organization"] = organization;
        
        if (url.has_value()) 
        {
            j["url"] = *url;
        }
        
        return j.dump();
    }

    AgentProvider AgentProvider::from_json(const std::string& json_str) 
    {
        try 
        {
            json j = json::parse(json_str);
            AgentProvider provider;
            
            provider.name = j.value("name", "");
            provider.organization = j.value("organization", "");
            
            if (j.contains("url") && j["url"].is_string()) 
            {
                provider.url = j["url"].get<std::string>();
            }
            
            return provider;
        } 
        catch (const json::exception&) 
        {
            throw A2AException("Failed to parse AgentProvider JSON", ErrorCode::ParseError);
        }
    }

    std::string AgentCard::to_json() const 
    {
        json j;
        
        j["name"] = name_;
        j["description"] = description_;
        j["url"] = url_;
        j["version"] = version_;
        j["protocolVersion"] = protocol_version_;
        
        // 嵌套对象转换：调用子模块的 to_json 然后 parse 成 JSON 对象塞进去
        j["capabilities"] = json::parse(capabilities_.to_json());
        
        j["defaultInputModes"] = default_input_modes_;
        j["defaultOutputModes"] = default_output_modes_;
        
        // 处理技能数组（对象数组）
        json skills_array = json::array();
        for (const auto& skill : skills_) 
        {
            skills_array.push_back(json::parse(skill.to_json()));
        }
        j["skills"] = skills_array;
        
        // 处理枚举类型
        j["preferredTransport"] = (preferred_transport_ == AgentTransport::JsonRpc ? "jsonrpc" : "http");
        
        // 处理可选字段 (Optionals)
        if (icon_url_.has_value())
         {
            j["iconUrl"] = *icon_url_;
        }
        if (documentation_url_.has_value()) 
        {
            j["documentationUrl"] = *documentation_url_;
        }
        if (provider_.has_value()) 
        {
            j["provider"] = json::parse(provider_->to_json());
        }
        
        return j.dump();
    }

    AgentCard AgentCard::from_json(const std::string& json_str) 
    {
        try 
        {
            json j = json::parse(json_str);
            AgentCard card;
            
            card.name_ = j.value("name", "");
            card.description_ = j.value("description", "");
            card.url_ = j.value("url", "");
            card.version_ = j.value("version", "");
            card.protocol_version_ = j.value("protocolVersion", "0.3.0");
            
            if (j.contains("capabilities") && j["capabilities"].is_object()) 
            {
                card.capabilities_ = AgentCapabilities::from_json(j["capabilities"].dump());
            }
            
            if (j.contains("defaultInputModes") && j["defaultInputModes"].is_array()) 
            {
                card.default_input_modes_ = j["defaultInputModes"].get<std::vector<std::string>>();
            }
            
            if (j.contains("defaultOutputModes") && j["defaultOutputModes"].is_array())
            {
                card.default_output_modes_ = j["defaultOutputModes"].get<std::vector<std::string>>();
            }
            
            if (j.contains("skills") && j["skills"].is_array()) 
            {
                for (const auto& skill_json : j["skills"]) {
                    card.skills_.push_back(AgentSkill::from_json(skill_json.dump()));
                }
            }
            
            if (j.contains("preferredTransport") && j["preferredTransport"].is_string())
            {
                if (j["preferredTransport"] == "http") 
                {
                    card.preferred_transport_ = AgentTransport::Http;
                } 
                else 
                {
                    card.preferred_transport_ = AgentTransport::JsonRpc;
                }
            }
            
            if (j.contains("iconUrl") && j["iconUrl"].is_string()) 
            {
                card.icon_url_ = j["iconUrl"].get<std::string>();
            }
            
            if (j.contains("documentationUrl") && j["documentationUrl"].is_string()) 
            {
                card.documentation_url_ = j["documentationUrl"].get<std::string>();
            }
            
            if (j.contains("provider") && j["provider"].is_object()) 
            {
                card.provider_ = AgentProvider::from_json(j["provider"].dump());
            }
            
            return card;
        } 
        catch (const json::exception&)
        {
            throw A2AException("Failed to parse AgentCard JSON", ErrorCode::ParseError);
            return AgentCard();
        }
    }

} // namespace a2a