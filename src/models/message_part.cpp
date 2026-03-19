#include "../../include/models/message_part.hpp"
#include "../../include/core/types.hpp"
#include "../../include/core/exception.hpp"
#include <nlohmann/json.hpp>
#include <vector>
#include <string>

using json = nlohmann::json;

namespace a2a
{
    //把任何杂乱无章的二进制字节，翻译成只包含 64 个安全字符（A-Z, a-z, 0-9, +, /）的纯文本
    static const std::string base64_chars = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    static std::string base64_encode(const std::vector<uint8_t>& data)
    {
        // 3 个普通字节  --》 4 个 Base64 字符
        //    8 bit              6 bit
        std::string ret;
        int i = 0, j = 0;
        uint8_t char_array_3[3];
        uint8_t char_array_4[4];
        size_t in_len = data.size();
        const uint8_t* bytes_to_encode = data.data();

        while(in_len--)
        {
            char_array_3[i++] = *(bytes_to_encode++);
            if(i == 3)
            {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for(i = 0; i <4; i++)
                    ret += base64_chars[char_array_4[i]];
                i = 0;
            }
        }
        if(i)
        {
            for(j = i; j < 3; j++) char_array_3[j] = '\0';
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            for (j = 0; j < i + 1; j++) ret += base64_chars[char_array_4[j]];
            while((i++ < 3)) ret += '=';
        }
        return ret;
    }


    static std::vector<uint8_t> base64_decode(const std::string& encoded_string) 
    {
        int in_len = encoded_string.size();
        int i = 0, j = 0, in_ = 0;
        uint8_t char_array_4[4], char_array_3[3];
        std::vector<uint8_t> ret;

        while (in_len-- && (encoded_string[in_] != '=') && 
              (isalnum(encoded_string[in_]) || (encoded_string[in_] == '+') || (encoded_string[in_] == '/'))) 
        {
            char_array_4[i++] = encoded_string[in_]; in_++;
            if (i == 4) 
            {
                for (i = 0; i < 4; i++) char_array_4[i] = base64_chars.find(char_array_4[i]);
                char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
                char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
                char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
                for (i = 0; (i < 3); i++) ret.push_back(char_array_3[i]);
                i = 0;
            }
        }
        if (i) 
        {
            for (j = i; j < 4; j++) char_array_4[j] = 0;
            for (j = 0; j < 4; j++) char_array_4[j] = base64_chars.find(char_array_4[j]);
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
            for (j = 0; (j < i - 1); j++) ret.push_back(char_array_3[j]);
        }
        return ret;
    }

    std::string TextPart::to_json() const 
    {
        json j;
        j["kind"] = "text";
        j["text"] = text_;  
        return j.dump();
    }

    std::string FilePart::to_json() const 
    {
        json j;
        j["kind"] = "file";
        j["file"] = {
            {"filename", filename_},
            {"mimeType", mime_type_},
            {"data", base64_encode(data_)} // 二进制必须转为 Base64 纯文本
        };
        return j.dump();
    }

    std::string DataPart::to_json() const 
    {
        json j;
        j["kind"] = "data";
        // 因为 data_json_ 本身就是 JSON 字符串，必须用 parse 解析成对象，防止被加上外层双引号
        j["data"] = json::parse(data_json_); 
        return j.dump();
    }

    std::unique_ptr<Part> Part::from_json(const std::string& json_str) 
    {
        try {
            json j = json::parse(json_str);
            
            // 安全获取类型
            std::string kind = j.value("kind", "");
            
            if (kind == "text")
            {
                std::string text = j.value("text", "");
                return std::make_unique<TextPart>(text);
            } 
            else if (kind == "file") 
            {
                // 现在可以完美解析文件了，告别原代码的 "假解析"
                if (j.contains("file") && j["file"].is_object()) {
                    auto& f = j["file"];
                    std::string filename = f.value("filename", "unknown.dat");
                    std::string mime_type = f.value("mimeType", "application/octet-stream");
                    std::string base64_data = f.value("data", "");
                    
                    return std::make_unique<FilePart>(filename, mime_type, base64_decode(base64_data));
                }
            }
            else if (kind == "data") 
            {
                if (j.contains("data")) 
                {
                    // 把结构化的 data 对象重新 dump 成纯字符串传入
                    return std::make_unique<DataPart>(j["data"].dump());
                }
            }
            
            return nullptr;
            
        }
        catch (const json::exception& e)
        {
            throw A2AException(std::string("Failed to parse Part from JSON: ") + e.what(), ErrorCode::ParseError);
        }
    }
} // namespace a2a