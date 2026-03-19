#pragma once

#include <string>
#include <optional>
#include <map>

namespace a2a 
{

    // 产物 (Artifact) 是智能体在执行任务过程中产生的结果，
    // 可以是任何类型的数据，如文本、文件、图片、链接等。
    // 它是智能体与外界交互的载体，也是智能体内部知识积累和传递的媒介。
class Artifact
{
public:
    Artifact() = default;
    
    Artifact(const std::string& id, const std::string& name)
        : id_(id), name_(name) {}
    
    // Getters
    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::optional<std::string>& description() const { return description_; }
    const std::optional<std::string>& mime_type() const { return mime_type_; }
    const std::optional<std::string>& url() const { return url_; }
    const std::optional<std::string>& content() const { return content_; }
    const std::map<std::string, std::string>& metadata() const { return metadata_; }
    
    // Setters
    void set_id(const std::string& id) { id_ = id; }
    void set_name(const std::string& name) { name_ = name; }
    void set_description(const std::string& desc) { description_ = desc; }
    void set_mime_type(const std::string& type) { mime_type_ = type; }
    void set_url(const std::string& url) { url_ = url; }
    void set_content(const std::string& content) { content_ = content; }
    void add_metadata(const std::string& key, const std::string& value) {
        metadata_[key] = value;
    }
    

    std::string to_json() const;
    
    static Artifact from_json(const std::string& json);
    static Artifact create() {return Artifact();}
    
    Artifact& with_id(const std::string& id) 
    {
        id_ = id;
        return *this;
    }
    
    Artifact& with_name(const std::string& name) 
    {
        name_ = name;
        return *this;
    }
    
    Artifact& with_description(const std::string& desc) 
    {
        description_ = desc;
        return *this;
    }
    
    Artifact& with_mime_type(const std::string& type) 
    {
        mime_type_ = type;
        return *this;
    }
    
    Artifact& with_url(const std::string& url) 
    {
        url_ = url;
        return *this;
    }
    
    Artifact& with_content(const std::string& content) 
    {
        content_ = content;
        return *this;
    }
    
    Artifact& with_metadata(const std::string& key, const std::string& value) 
    {
        metadata_[key] = value;
        return *this;
    }

private:
    std::string id_;
    std::string name_;
    std::optional<std::string> description_;
    std::optional<std::string> mime_type_;
    std::optional<std::string> url_;   // 应对大文件或敏感内容，智能体可以选择只存储一个 URL 链接，而不直接保存内容
    std::optional<std::string> content_;
    std::map<std::string, std::string> metadata_;
};

} // namespace a2a
