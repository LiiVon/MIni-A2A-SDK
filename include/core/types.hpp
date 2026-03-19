#pragma once

#include <string>
#include <vector>
#include <memory>
// 可能有、也可能没有的值
// has_value() + value() / value_or() 访问方式
#include <optional>  
#include <functional>
#include <map>
#include <chrono>


// 公共类型与基础字典头文件
// 为整个 A2A系统 定义一套统一的“词汇表”和“规则”
namespace a2a 
{

    // 前置声明。避免循环依赖，减少编译时间
    class AgentTask;
    class AgentMessage;
    class AgentCard;
    class MessageSendParams;
    class A2AResponse;

    // 类型别名
    using Timestamp = std::chrono::system_clock::time_point;
    using Json = std::map<std::string, std::string>; 
    using Callback = std::function<void()>;

    // 强类型枚举
    // 防止枚举值污染全局命名空间
    // 也防止不同枚举之间发生意外的隐式类型转换。
    enum class MessageRole  // 智能体角色
    {
        User,
        Agent,
        System
    };

    enum class TaskState    // 任务的状态机
    {
        Submitted,
        Running,
        Completed,
        Failed,
        Canceled,
        Rejected
    };

    enum class PartKind
    {
        Text,
        File,
        Data
    };

    enum class AgentTransport 
    {
        JsonRpc,
        Http
    };

    // 序列化与反序列化辅助函数
    inline std::string to_string(MessageRole role) 
    {
        switch (role)
        {
            case MessageRole::User: return "user";
            case MessageRole::Agent: return "agent";
            case MessageRole::System: return "system";
            default: return "unknown";
        }
    }

    inline std::string to_string(TaskState state) 
    {
        switch (state)
        {
            case TaskState::Submitted: return "submitted";
            case TaskState::Running: return "running";
            case TaskState::Completed: return "completed";
            case TaskState::Failed: return "failed";
            case TaskState::Canceled: return "canceled";
            case TaskState::Rejected: return "rejected";
            default: return "unknown";
        }
    }

    // C++ switch 语句不支持字符串，所以使用 if-else 来实现字符串到枚举的转换
    inline MessageRole message_role_from_string(const std::string& str) 
    {
        if (str == "user") return MessageRole::User;
        if (str == "agent") return MessageRole::Agent;
        if (str == "system") return MessageRole::System;
        return MessageRole::User;
    }

    inline TaskState task_state_from_string(const std::string& str) 
    {
        if (str == "submitted") return TaskState::Submitted;
        if (str == "running") return TaskState::Running;
        if (str == "completed") return TaskState::Completed;
        if (str == "failed") return TaskState::Failed;
        if (str == "canceled") return TaskState::Canceled;
        if (str == "rejected") return TaskState::Rejected;
        return TaskState::Submitted;
    }

} // namespace a2a
