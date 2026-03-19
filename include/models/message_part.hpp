#pragma once

#include "../core/types.hpp"
#include <string>
#include <memory>
#include <vector>

// 一个消息（Message）可以包含哪些类型的内容
namespace a2a
{
    // 基类，满足以后的 多态扩展 message part类型
    class Part
    {
        public:
        // 只要一个类有虚函数，它的析构函数必须是虚函数
        virtual ~Part() = default;
        virtual PartKind kind() const = 0;
        virtual std::string to_json() const = 0;
        virtual std::unique_ptr<Part> clone() const = 0;

        static std::unique_ptr<Part> from_json(const std::string& json_str);
    };

    // 纯文本消息块
    class TextPart :public Part
    {  
    public:
        TextPart() =default;
        // 单个参数 的构造函数可以使用 explicit 关键字修饰，防止隐式类型转换导致的错误。
        explicit TextPart(const std::string& text):text_(text) {}
        PartKind kind() const override { return PartKind::Text; }
        const std::string& text() const { return text_; }
        void set_text(const std::string& text) { text_ = text; }  

        std::string to_json() const override;
        // 克隆函数，返回一个新的对象，避免 slicing 问题
        std::unique_ptr<Part> clone() const override
        {
            return std::make_unique<TextPart>(*this);
        }
        
        private:
        std::string text_;
    };

    // 文件消息块
    class FilePart : public Part
    {
    public:
        FilePart() = default;
        FilePart(const std::string&filename,const std::string& mine_type,
                const std::vector<uint8_t>& data)
                :filename_(filename)
                ,mime_type_(mine_type)
                ,data_(data) {}

        PartKind kind() const override { return PartKind::File; }

        const std::string& filename() const { return filename_; }
        const std::string& mime_type() const { return mime_type_; }
        const std::vector<uint8_t>& data() const { return data_; }

        void set_filename(const std::string& filename) { filename_ = filename; }
        void set_mime_type(const std::string& mime_type) { mime_type_ = mime_type; }
        void set_data(const std::vector<uint8_t>& data) { data_ = data; }
        
        std::string to_json() const override;
        std::unique_ptr<Part> clone() const override
        {
            return std::make_unique<FilePart>(*this);
        }
        private:
        std::string filename_;
        std::string mime_type_;
        std::vector<uint8_t> data_;  // 二进制数据 
    };

    // @brief: Data message part -- for structured data like JSON
    class DataPart : public Part
    {
    public:
        DataPart() = default;
        explicit DataPart(const std::string& data_json):data_json_(data_json) {}

        PartKind kind() const override { return PartKind::Data; }

        const std::string& data_json() const { return data_json_; }
        void set_data_json(const std::string& data_json) { data_json_ = data_json; }

        std::string to_json() const override ;
        std::unique_ptr<Part> clone() const override
        {
            return std::make_unique<DataPart>(*this);
        }
    private:
        std::string data_json_;
    };
}