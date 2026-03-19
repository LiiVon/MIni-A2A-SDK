# Global Architecture & Flow

A2A C++ SDK 的设计目标是提供一个轻量级、高性能的 C++ 客户端库，用于与 A2A (Agent-to-Agent) 服务进行通信。整体架构基于 HTTP 协议之上的 JSON-RPC 2.0 标准。

## 1. 整体架构 (Overall Architecture)

SDK core 模块主要负责底层的通信协议封装和网络传输。上层业务逻辑通过调用 core 提供的基础能力来实现具体的业务功能（如发送消息、管理任务）。

### 数据流向 (Data Flow)

1. **Client Call**: 用户调用 SDK 提供的业务接口 (例如 `send_message`, `get_task`)。
2. **Request Packaging**: SDK 内部将用户的参数封装成标准的 **JSON-RPC Request** 对象。
    - Method: 使用 `A2AMethods` 中定义的常量 (如 `"message/send"`).
    - Params: 业务参数被序列化为 JSON 字符串。
    - ID: 生成唯一的请求 ID 用于匹配响应。
3. **Transport Layer**: `HttpClient` 接收封装好的请求数据。
    - `HttpClient` 负责建立连接、设置 Header (Content-Type: application/json)、处理超时。
    - 使用 `libcurl` 发送 HTTP POST 请求。
4. **Network Transmission**: 请求通过网络发送到 A2A Server。
5. **Response Handling**:
    - **Unary Call (单次调用)**: Server 返回完整的 JSON 响应，`HttpClient` 接收并返回给 SDK 层解析。
    - **Streaming Call (流式调用)**: 对于 `message/stream` 等接口，Server 分块返回数据 (SSE 或 Chunked)，`HttpClient` 通过回调函数 (`callback`) 实时将数据推送给上层。
6. **Error Handling**: SDK 层捕获网络错误或协议错误，抛出 `A2AException` 或返回错误码。

## 2. 核心模块概览 (Core Modules Overview)

- **Network**: `HttpClient` (基于 libcurl 的 HTTP 客户端)。
- **Protocol**: `JsonRpcRequest`, `JsonRpcResponse` (JSON-RPC 2.0 协议封装)。
- **Constants**: `A2AMethods` (API 方法名定义), `ErrorCode` (错误码定义)。
- **Types**: `types.hpp` (通用数据结构，如 MessageRole, TaskState)。
- **Error Handling**: `A2AException` (自定义异常体系)。

## 3. 开发路线图 (Roadmap)

1. ✅ **Core Infrastructure**: 完成 HttpClient, JSON-RPC 封装, 基础异常处理。
2. 🔄 **Service Layer**: 封装具体的业务接口 (MessageService, TaskService)。
3. 🔲 **Async Support**: 引入异步机制 (Future/Promise 或 Callback hell 优化)。
4. 🔲 **Testing**: 单元测试覆盖。

---
*Created per request for learning path tracking.*
