# Core Module Design Notes

本文件记录了 `src/core` 和 `include/core` 下各模块的详细设计思路与实现原因。

## 1. HttpClient (`http_client.*`)

A2A SDK 的核心通信模块，封装了底层的 HTTP 请求逻辑。

### 为什么使用 PIMPL (Pointer to Implementation)?

- **ABI 稳定性**: `HttpClient` 的私有成员（如 `curl_global_init` 相关状态）被隐藏在 `Impl` 结构体中，只在 `.cpp` 文件定义。修改内部实现不需要重新编译所有依赖该头文件的模块。
- **减少依赖**: `include/core/http_client.hpp` 不需要包含 `#include <curl/curl.h>`，避免将 `libcurl` 的宏定义和类型污染给用户代码。
- **编译加速**: 用户只需要包含简单的头文件，减少预处理时间。

### 为什么设计 `post_stream` 回调机制?

- **实时性**: 对于像 `message/stream` 这样的流式 API，服务器可能分块返回数据（如 SSE - Server-Sent Events）。
- **内存优化**: 避免将巨大的响应体一次性加载到内存字符串中，而是通过回调逐块处理数据。
- **灵活性**: 允许上层业务逻辑在收到部分数据时立即做出反应（如聊天机器人的打字机效果）。

### 为什么在 `write_callback` 中使用 `std::string`追加?

- **简单**: 为了兼容同步调用（`get`, `post`），直接将所有数据拼接成完整字符串返回是最直观的方式。
- **通用**: 适用于非流式且响应体较小的常规 API。

## 2. JSON-RPC Protocol (`jsonrpc_request.*`, `jsonrpc_response.*`)

A2A 通信协议基于 JSON-RPC 2.0 标准。

### 设计考量

- **标准化**: `jsonrpc` 版本号固定为 `"2.0"`，符合协议规范。
- **结构化参数**: `params_json` 作为一个单独的 JSON 字符串处理，而不是强类型绑定。这提供了灵活性，允许上层根据具体的 `method` 动态构造参数结构，而在 Transport 层统一视为字符串传输。
- **ID 管理**: 必须包含 `id` 字段以支持请求-响应匹配。

## 3. API 方法常量 (`a2a_methods.hpp`)

### 为什么使用 `static constexpr`?

- **编译期检查**: 避免在代码中到处硬编码字符串（如 `"message/send"`），减少拼写错误风险。
- **单一真值**: 所有 API 方法名集中管理，方便后续维护和查找引用。
- **辅助函数**: `is_streaming_method` 提供逻辑判断，封装了哪些方法需要流式处理的业务规则，避免在 `HttpClient` 调用处散落 `if-else`。

## 4. 异常处理 (`exception.*`)

### 设计思路

- **继承自 `std::exception`**: 符合 C++ 标准异常体系，能被标准库容器和算法兼容。
- **错误码 (`ErrorCode`)**: 携带具体的业务错误类型（如 `NetworkError`, `ProtocolError`），而不只是通用的文本描述，方便上层做错误分类处理。
- **构造函数**: 提供多种构造方式，支持仅错误码或带详细描述。

## 5. 类型定义 (`types.hpp`)

### 设计思路

- **强类型枚举 (`enum class`)**: 如 `MessageRole`, `TaskState`。避免整数隐式转换带来的风险，提高代码语义清晰度。
- **别名 (`using`)**: 简化常用复杂类型（如 `using Json = std::map<std::string,std::string>;`），但注意需配合 JSON 库使用以支持嵌套结构。

---
*Created per request for core component analysis.*
