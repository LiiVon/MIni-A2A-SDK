#include "../include/core/jsonrpc_request.hpp"
#include "../include/core/jsonrpc_response.hpp"
#include "../include/core/exception.hpp"
#include <iostream>
#include <cassert>

using namespace a2a;

void test_request_serialization()
{
    std::cout<<" ====测试1：JSON-RPC Request 序列化==== "<<std::endl;
    JsonRpcRequest req;
    req.set_id("123");
    req.set_method("a2a.message.send");
    req.set_params_json(R"({"message":{"role":"user","text":"Hello"}})");

    std::string json = req.to_json();
    std::cout<<"生成的JSON: "<<json<<std::endl;

    assert(json.find(R"("jsonrpc":"2.0")") != std::string::npos);
    assert(json.find(R"("id":"123")") != std::string::npos);
    assert(json.find(R"("method":"a2a.message.send")") != std::string::npos);
    assert(json.find("\"method\":\"a2a.message.send\"") != std::string::npos);
    std::cout<<"测试1通过！"<<std::endl;
}

int main()
{
    std::cout << "╔══════════════════════════════════════════╗" << std::endl;
    std::cout << "║  A2A C++ SDK - JSON-RPC 功能验证测试     ║" << std::endl;
    std::cout << "║  使用 nlohmann::json 库                  ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;

    try
    {
        test_request_serialization();
        return 0;
    }
    catch(const std::exception& e)
    {
        std::cerr << "测试失败: " << e.what() << '\n';
        return 1;
    }
    
}