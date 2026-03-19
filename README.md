# Mini-A2A-SDK 
基于 A2A (Agent-to-Agent) 协议的 C++ 实现，提供构建和连接智能 Agent 的完整框架。

## 架构流程
![frame.png](https://raw.githubusercontent.com/LiiVon/Mini-A2A-SDK/main/frame.png)


## 工作流程
![run.png](https://raw.githubusercontent.com/LiiVon/Mini-A2A-SDK/main/run.png)

## ✨ 核心特性

### SDK 功能

- ✅ **完整的 A2A 协议实现**
  - JSON-RPC 2.0 消息格式
  - Agent Card 元数据
  - 上下文管理（contextId）
  - 历史长度控制（history_length）

- ✅ **灵活的 TaskStore**
  - 内存实现（MemoryTaskStore）：适合单机开发
  - Redis 实现（RedisTaskStore）：适合生产环境
  - 可扩展接口（ITaskStore）：支持自定义实现

- ✅ **服务注册与发现**
  - 自动服务注册
  - 按标签查找服务
  - 健康检查机制
  - 负载均衡（轮询）

- ✅ **生产级特性**
  - 线程安全设计
  - HTTP/HTTPS 传输
  - 自动重连机制
  - 错误处理和日志
