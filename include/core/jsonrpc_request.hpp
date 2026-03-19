#pragma once

#include <string>
#include <optional>
#include <memory>

namespace a2a 
{
// 表示一个标准的 JSON-RPC 2.0 网络请求对象
class JsonRpcRequest 
{
public:
    JsonRpcRequest() = default;
    
    JsonRpcRequest(const std::string& id, 
                   const std::string& method,
                   const std::string& params_json = "{}")
        : jsonrpc_("2.0")
        , id_(id)
        , method_(method)
        , params_json_(params_json) {}
    
    // Getters  &性能好 ，否则得拷贝
    const std::string& jsonrpc() const { return jsonrpc_; }
    const std::string& id() const { return id_; }
    const std::string& method() const { return method_; }
    const std::string& params_json() const { return params_json_; }
    
    // Setters
    void set_id(const std::string& id) { id_ = id; }
    void set_method(const std::string& method) { method_ = method; }
    void set_params_json(const std::string& params) { params_json_ = params; }
    
    // Serialization and deserialization
    std::string to_json() const;
    static JsonRpcRequest from_json(const std::string& json);

private:
// json协议规定四个字段：jsonrpc、id、method、params
    std::string jsonrpc_ = "2.0";
    std::string id_;
    std::string method_;
    std::string params_json_ = "{}";
};

} 
