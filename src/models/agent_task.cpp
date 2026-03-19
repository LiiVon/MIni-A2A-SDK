#include "../../include/models/agent_task.hpp"
#include "../../include/core/exception.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace a2a 
{

    std::string AgentTask::to_json() const 
    {
        json j;
        j["id"] = id_;
        j["contextId"] = context_id_;
        
        // status_.to_json() 返回的是纯文本字符串
        // 必须用 json::parse 把它变回 JSON 对象再塞进去，防止外层被加上双引号转义
        j["status"] = json::parse(status_.to_json());
        
        if (!artifacts_.empty()) 
        {
            json arr = json::array(); // 创建一个空的 JSON 数组
            for (const auto& a : artifacts_) 
            {
                arr.push_back(json::parse(a.to_json()));
            }
            j["artifacts"] = arr;
        }
        
        if (!history_.empty()) 
        {
            json arr = json::array();
            for (const auto& h : history_) 
            {
                arr.push_back(json::parse(h.to_json()));
            }
            j["history"] = arr;
        }
        
        if (!metadata_.empty()) 
        {
            j["metadata"] = metadata_;
        }
        
        return j.dump(); // 完美打包
    }

    AgentTask AgentTask::from_json(const std::string& json_str) 
    {
        try 
        {
            json j = json::parse(json_str);
            AgentTask task;

            task.id_ = j.value("id", "");
            task.context_id_ = j.value("contextId", "");
            
            if (j.contains("status") && j["status"].is_object()) 
            {
                task.status_ = AgentTaskStatus::from_json(j["status"].dump());
            }
            
            if (j.contains("artifacts") && j["artifacts"].is_array()) 
            {
                for (const auto& item : j["artifacts"]) 
                {
                    // 把数组里的每一个元素，扔给 Artifact 自己去解析
                    task.artifacts_.push_back(Artifact::from_json(item.dump()));
                }
            }
            
            if (j.contains("history") && j["history"].is_array()) 
            {
                for (const auto& item : j["history"]) 
                {
                    // AgentMessage 内部也是多态的，但这里不用管，扔给它自己去处理
                    task.history_.push_back(AgentMessage::from_json(item.dump()));
                }
            }
            
            if (j.contains("metadata") && j["metadata"].is_object()) 
            {
                task.metadata_ = j["metadata"].get<std::map<std::string, std::string>>();
            }
            
            return task;
            
        } 
        catch (const json::exception& e) 
        {
            throw A2AException(std::string("Failed to parse AgentTask from JSON") + e.what(),
                ErrorCode::ParseError);
        }
    }

} // namespace a2a