#include "../../include/models/artifact.hpp"
#include "../../include/core/exception.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace a2a 
{

std::string Artifact::to_json() const 
{
    json j;
    j["id"] = id_;
    j["name"] = name_;
    
    // 选填字段 (Optionals)
    if (description_.has_value()) j["description"] = *description_;
    if (mime_type_.has_value()) j["mimeType"] = *mime_type_;
    if (url_.has_value()) j["url"] = *url_;
    if (content_.has_value()) j["content"] = *content_;
    
    // 字典典字段 (Metadata)
    if (!metadata_.empty()) 
    {
        j["metadata"] = metadata_;
    }
    
    return j.dump(); // 自动处理所有转义，生成完美合法的 JSON
}

Artifact Artifact::from_json(const std::string& json_str) 
{
    try 
    {
        json j = json::parse(json_str);
        Artifact artifact;
        
        // 提取必填字段（带有默认值兜底，防崩溃）
        artifact.id_ = j.value("id", "");
        artifact.name_ = j.value("name", "");
        
        // 提取选填字段
        if (j.contains("description") && j["description"].is_string()) 
        {
            artifact.description_ = j["description"].get<std::string>();
        }
        
        if (j.contains("mimeType") && j["mimeType"].is_string()) 
        {
            artifact.mime_type_ = j["mimeType"].get<std::string>();
        }
        
        if (j.contains("url") && j["url"].is_string()) 
        {
            artifact.url_ = j["url"].get<std::string>();
        }
        
        if (j.contains("content") && j["content"].is_string()) 
        {
            artifact.content_ = j["content"].get<std::string>();
        }
        
        // 3. 提取字典 (Metadata)
        if (j.contains("metadata") && j["metadata"].is_object()) 
        {
            artifact.metadata_ = j["metadata"].get<std::map<std::string, std::string>>();
        }
        
        return artifact;
        
    } 
    catch (const json::exception& e) 
    {
        throw A2AException(std::string("Failed to parse Artifact from JSON") + e.what(),
             ErrorCode::ParseError);
    }
}

} // namespace a2a