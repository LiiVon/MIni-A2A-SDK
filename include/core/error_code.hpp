#pragma once

#include <cstdint>

namespace a2a 
{

    // 确保在任何操作系统、任何编译器下，这个枚举都严格占用 4 个字节。
    // 这对于网络通信序列化至关重要
enum class ErrorCode : int32_t
// enum 强类型 
// 防止枚举值污染全局命名空间，也防止不同枚举之间发生意外的隐式类型转换。
 {
    // JSON-RPC 2.0 官方规范 错误码
    ParseError = -32700,
    InvalidRequest = -32600,
    MethodNotFound = -32601,
    InvalidParams = -32602,
    InternalError = -32603,
    
    // JSON 协议-32000 到 -32099 作为自定义错误码范围
    // A2A 自定义错误码
    TaskNotFound = -32001,
    TaskNotCancelable = -32002,
    UnsupportedOperation = -32003,
    ContentTypeNotSupported = -32004,
    PushNotificationNotSupported = -32005
};


// 返回 const char* 可以避免不必要的 string类 对象构造和析构，提高性能，
// 且错误码 一般不需要用户修改，返回 const char* 可以明确表达这一点。
// string_view 也可以避免复制

// inline 头文件实现函数，避免链接错误
// C++17 inline可以修饰 静态全局变量，避免在类中声明，在cpp中定义的麻烦
// 类内的成员函数 或者 模板函数 自带 隐式inline
inline const char* error_code_to_string(ErrorCode code) 
{
    switch (code) 
    {
        case ErrorCode::ParseError:
            return "Parse error";
        case ErrorCode::InvalidRequest:
            return "Invalid request";
        case ErrorCode::MethodNotFound:
            return "Method not found";
        case ErrorCode::InvalidParams:
            return "Invalid params";
        case ErrorCode::InternalError:
            return "Internal error";
        case ErrorCode::TaskNotFound:
            return "Task not found";
        case ErrorCode::TaskNotCancelable:
            return "Task not cancelable";
        case ErrorCode::UnsupportedOperation:
            return "Unsupported operation";
        case ErrorCode::ContentTypeNotSupported:
            return "Content type not supported";
        case ErrorCode::PushNotificationNotSupported:
            return "Push notification not supported";
        default:
            return "Unknown error";
    }
}

} // namespace a2a
