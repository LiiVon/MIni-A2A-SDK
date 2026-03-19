#include "../../include/models/task_status.hpp"
#include "../../include/core/exception.hpp"
#include <nlohmann/json.hpp>
#include <sstream>
#include <iomanip>
#include <ctime>

using json = nlohmann::json;

namespace a2a 
{
    // 辅助函数 把 C++ 时间戳变成 ISO 8601 字符串 (保持原样)
    // 例如：2023-10-25T14:30:00.123Z
    static std::string timestamp_to_iso8601(const Timestamp& ts) 
    {
        // 毫秒级 转成 秒级 time_t
        auto time_t_val = std::chrono::system_clock::to_time_t(ts);
        std::tm tm_val; 
        // tm结构体 含year、month、day、hour、min、sec等字段
        
    #ifdef _WIN32
        gmtime_s(&tm_val, &time_t_val);
    #else
        gmtime_r(&time_t_val, &tm_val);
    #endif
        
        std::ostringstream oss;
        oss << std::put_time(&tm_val, "%Y-%m-%dT%H:%M:%S");
        
        // 找回 毫秒
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            ts.time_since_epoch()
        ) % 1000;
        
        oss << "." << std::setfill('0') << std::setw(3) << ms.count() << "Z";
        return oss.str();
    }


    //  辅助函数 ：把字符串变回 C++ 时间戳
    static Timestamp iso8601_to_timestamp(const std::string& time_str) 
    {
        std::tm tm_val = {};
        std::istringstream ss(time_str);
        
        // 按照格式把年月日时分秒塞进 tm_val
        ss >> std::get_time(&tm_val, "%Y-%m-%dT%H:%M:%S");
        
        if (ss.fail()) 
        {
            // 如果对方传来的时间格式不对，作为兜底才使用当前时间
            return std::chrono::system_clock::now();
        }

        // 将 UTC 时间转换为 C++ 底层的 time_t 
    #ifdef _WIN32
        time_t t = _mkgmtime(&tm_val);
    #else
        time_t t = timegm(&tm_val);
    #endif

        auto tp = std::chrono::system_clock::from_time_t(t);

        // 提取毫秒（如果字符串里包含小数点）
        size_t dot_pos = time_str.find('.');
        if (dot_pos != std::string::npos) 
        {
            try 
            {
                int ms = std::stoi(time_str.substr(dot_pos + 1, 3));
                tp += std::chrono::milliseconds(ms);
            } 
            catch (...) {} // 忽略毫秒转换的潜在异常
        }

        return tp;
    }

    std::string AgentTaskStatus::to_json() const 
    {
        json j;
        
        // 假设有 to_string 函数处理枚举
        j["state"] = to_string(state_); 
        
        // 调用时间转换函数
        j["timestamp"] = timestamp_to_iso8601(timestamp_);
        
        if (!message_.empty()) 
        {
            j["message"] = message_;
        }
        
        return j.dump();
    }

    AgentTaskStatus AgentTaskStatus::from_json(const std::string& json_str) 
    {
        try 
        {
            json j = json::parse(json_str);
            AgentTaskStatus status;
            
            if (j.contains("state") && j["state"].is_string()) 
            {
                status.state_ = task_state_from_string(j["state"].get<std::string>());
            }
            
            if (j.contains("timestamp") && j["timestamp"].is_string()) 
            {
                status.timestamp_ = iso8601_to_timestamp(j["timestamp"].get<std::string>());
            } else 
            {
                status.timestamp_ = std::chrono::system_clock::now(); // 只有没传时间时才用当前时间
            }

            if (j.contains("message") && j["message"].is_string()) 
            {
                status.message_ = j["message"].get<std::string>();
            }
            
            return status;
            
        } 
        catch (const json::exception& e) 
        {
            throw A2AException(std::string("Failed to parse AgentTaskStatus from JSON") + e.what(),
                ErrorCode::ParseError);
        }
    }

} // namespace a2a