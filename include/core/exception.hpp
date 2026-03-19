#pragma once
#include "error_code.hpp"
#include <stdexcept>
#include <string>

namespace a2a 
{
    // 抛出异常错误码和请求追踪ID
    // std::exception --> std::runtime_error --> A2AException
class A2AException : public std::runtime_error 
{
public:
    A2AException(const std::string& message, ErrorCode code)
        : std::runtime_error(message)
        , error_code_(code)
        , request_id_() {}
    // 初始化列表直接初始化，{}需要赋值，性能更高
    
  
    A2AException(const std::string& message, ErrorCode code, const std::string& request_id)
        : std::runtime_error(message)
        , error_code_(code)
        , request_id_(request_id) {}
    

    // Getters
    // noexcept 说明这个函数不会抛出异常，可以让编译器进行优化。
    // 避免异常类抛出异常的时候 也出现异常，导致程序崩溃。
    ErrorCode error_code() const noexcept
    {
        return error_code_;
    }
    
    const std::string& request_id() const noexcept 
    {
        return request_id_;
    }
    
    int32_t error_code_value() const noexcept 
    {
        return static_cast<int32_t>(error_code_);
    }

private:
    ErrorCode error_code_;
    std::string request_id_;
};

} 
