#pragma once

#include "error_code.hpp"
#include <string>
#include <optional>

namespace a2a 
{
/* json规定 服务端响应必须是非黑即白
成功时：必须包含 result 字段，绝对不能有 error 字段。 
{"jsonrpc": "2.0", "id": "1", "result": {"status": "ok"}}
失败时：必须包含 error 字段，绝对不能有 result 字段。 
{"jsonrpc": "2.0", "id": "1", "error": {"code": -32600, "message": "Invalid"}}
*/
struct JsonRpcError 
{
    int32_t code;
    std::string message;
    std::string data;  // Optional additional data
    
    JsonRpcError() : code(0) {}
    JsonRpcError(int32_t c, const std::string& msg) 
        : code(c), message(msg) {}
    JsonRpcError(ErrorCode ec, const std::string& msg)
        : code(static_cast<int32_t>(ec)), message(msg) {}
};

class JsonRpcResponse 
{
public:
    JsonRpcResponse() = default;
    
    // Success response
    JsonRpcResponse(const std::string& id, const std::string& result_json)
        : jsonrpc_("2.0")
        , id_(id)
        , result_json_(result_json)
        , error_() {}
    
    // Error response
    JsonRpcResponse(const std::string& id, const JsonRpcError& error)
        : jsonrpc_("2.0")
        , id_(id)
        , result_json_()
        , error_(error) {}
    
    // Getters
    const std::string& jsonrpc() const { return jsonrpc_; }
    const std::string& id() const { return id_; }
    const std::optional<std::string>& result_json() const { return result_json_; }
    const std::optional<JsonRpcError>& error() const { return error_; }
    
    bool is_error() const { return error_.has_value(); }
    bool is_success() const { return result_json_.has_value(); }
    

    std::string to_json() const;
    
 
    static JsonRpcResponse from_json(const std::string& json);
    
    
    static JsonRpcResponse create_error(const std::string& id, 
                                       ErrorCode code, 
                                       const std::string& message);
    
    static JsonRpcResponse create_success(const std::string& id, 
                                         const std::string& result_json);

private:
    std::string jsonrpc_ = "2.0";
    std::string id_;
    // 成功失败不可能同时，使用 optional 来区分，避免了使用指针带来的内存管理问题，同时也更清晰地表达了这两者的互斥关系。
    std::optional<std::string> result_json_;
    std::optional<JsonRpcError> error_;
};

} 
