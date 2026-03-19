#pragma once 

#include <string>
#include <map>
#include <memory>
#include <functional>

namespace a2a
{
    // @brief : HTTP Response struct
    struct HttpResponse
    {
        int status_code;
        std::string body;
        std::map<std::string, std::string> headers;

        bool is_success() const 
        {
            return status_code >= 200 && status_code < 300;
        }
    };

    // @brief:HTTP Client interface
    class HttpClient
    {
    public:
        HttpClient() ;
        ~HttpClient() ;

        
        HttpClient(const HttpClient&) = delete;
        HttpClient& operator=(const HttpClient&) = delete;
        HttpClient(HttpClient&&) noexcept;
        HttpClient& operator=(HttpClient&&) noexcept;
    
   
        HttpResponse get(const std::string& url);

        HttpResponse post(const std::string&url,
                        const std::string& body,
                        const std::string& content_type = "application/json");

         void post_stream(const std::string&url,
                        const std::string& body,
                        const std::string& content_type,
                     std::function<void(const std::string&)> callback);

        void set_timeout(long seconds); 
        void add_header(const std::string& key, const std::string& value);
        void clear_headers();
    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };
}