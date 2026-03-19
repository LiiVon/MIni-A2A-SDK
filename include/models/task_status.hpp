#pragma once

#include "../core/types.hpp"
#include <string>
#include <chrono>

namespace a2a 
{
    // 任务状态类，包含状态枚举、时间戳和可选的状态消息
class AgentTaskStatus 
{
public:
    AgentTaskStatus() 
        : state_(TaskState::Submitted)
        , timestamp_(std::chrono::system_clock::now()) {}
    
    explicit AgentTaskStatus(TaskState state)
        : state_(state)
        , timestamp_(std::chrono::system_clock::now()) {}
    
    AgentTaskStatus(TaskState state, const Timestamp& timestamp)
        : state_(state)
        , timestamp_(timestamp) {}
    
    // Getters
    TaskState state() const { return state_; }
    const Timestamp& timestamp() const { return timestamp_; }
    const std::string& message() const { return message_; }
    
    // Setters
    void set_state(TaskState state) { state_ = state; }
    void set_timestamp(const Timestamp& ts) { timestamp_ = ts; }
    void set_message(const std::string& msg) { message_ = msg; }
    
    // 要不要继续等待？如果状态是 Completed、Failed、Canceled 或 Rejected，就不等了，直接返回结果；
    // 如果状态是 Submitted 或 Running，就继续等。
    bool is_terminal() const 
    {
        return state_ == TaskState::Completed ||
               state_ == TaskState::Failed ||
               state_ == TaskState::Canceled ||
               state_ == TaskState::Rejected;
    }
    

    std::string to_json() const;
    static AgentTaskStatus from_json(const std::string& json);

private:
    TaskState state_;
    Timestamp timestamp_;
    std::string message_;  // Optional status message
};

} // namespace a2a
