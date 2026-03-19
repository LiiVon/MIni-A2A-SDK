#pragma once
#include <string>

namespace a2a
{
    // 集中管理 A2A 系统中 JSON-RPC 通信的“方法名”
    class A2AMethods
    {
    public:
        // constexpt 编译期常量，运行时0开销
        static constexpr const char* MESSAGE_SEND = "message/send";
        static constexpr const char* MESSAGE_STREAM = "message/stream";

        
        static constexpr const char* TASK_GET = "task/get";
        static constexpr const char* TASK_CANCEL = "task/cancel";
        static constexpr const char* TASK_SUBSCRIBE = "task/resubscribe";

        
        static constexpr const char* TASK_PUSH_NOTIFICATION_CONFIG_SET = 
            "task/pushNotificationConfig/set";

        static constexpr const char* TASK_PUSH_NOTIFICATION_CONFIG_GET =    
            "task/pushNotificationConfig/get";

        
        static bool is_streaming_method(const std::string& method)
        {
            return method == MESSAGE_STREAM || method == TASK_SUBSCRIBE;
        }

        
        static bool is_valid_method(const std::string& method)
        {
            return method == MESSAGE_SEND ||
                     method == MESSAGE_STREAM ||
                     method == TASK_GET ||
                     method == TASK_CANCEL ||
                     method == TASK_SUBSCRIBE ||
                     method == TASK_PUSH_NOTIFICATION_CONFIG_SET ||
                     method == TASK_PUSH_NOTIFICATION_CONFIG_GET;
        }
    
    };
}