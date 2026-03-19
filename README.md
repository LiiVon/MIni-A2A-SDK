# Mini-A2A-SDK 
基于 A2A (Agent-to-Agent) 协议的 C++ 实现，提供构建和连接智能 Agent 的完整框架。

## 架构流程
![frame.png](https://raw.githubusercontent.com/LiiVon/Mini-A2A-SDK/main/frame.png)


## 工作流程
![run.png](https://raw.githubusercontent.com/LiiVon/Mini-A2A-SDK/main/run.png)

🌟 核心特性 (Features)
✅ 完整的 A2A 协议兼容
严格遵循 JSON-RPC 2.0 消息格式规范。
内置 Agent Card 元数据管理与 Context ID 上下文追踪。
支持自定义历史消息长度（history_length）控制。
🗄️ 多场景任务存储 (TaskStore)
MemoryTaskStore: 轻量级内存实现，零依赖，适合本地开发与调试。
RedisTaskStore: 分布式持久化实现，支持高并发与数据恢复，适合生产环境。
标准化接口: ITaskStore 抽象接口，支持自定义存储实现（如 MySQL、MongoDB）。
🤝 智能服务注册与发现
支持服务自动注册到注册中心。
基于标签（Tag）的精准服务查找与路由。
内置健康检查（Health Check）与心跳机制，确保服务可用性。
集成轮询（Round-Robin）负载均衡策略，提高系统可用性。
🏗️ 生产级可靠性设计
线程安全: 全链路并发安全设计，支持多线程 / 协程高并发访问。
传输可靠: 基于 HTTP/HTTPS 协议，支持超时设置与自动重连。
完善的容错: 统一的错误码体系、详细的日志记录与异常捕获机制。
