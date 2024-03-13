# ReactorNetX：基于reactor模式的高并发低延迟C++网络库

## 项目简介
`ReactorNetX`是一款高性能、多线程的C++网络库，采用`one thread one loop`架构和多`reactor`机制，实现了高并发请求的高效处理。该库还内置了一个简易的HTTP服务器，支持基础HTTP服务功能，使得构建高性能WebServer变得轻松。

## 主要特性
- **高性能**：采用`one thread one loop`及多`reactor`机制，优化并发请求处理。
- **简易HTTP服务器**：支持快速部署，提供基础的HTTP服务功能。
- **Server模块**：优化的Buffer管理和EventLoop设计，确保高效的连接分配和通信处理。
- **HTTP协议处理**：使用有限状态机解析请求，支持多种HTTP方法、路由分发，并实现响应的快速封装。
- **高并发支持**：在标准测试环境下，实现上万级别并发处理及15.4万QPS的处理速度。

## 使用场景
- 构建高性能、高并发的Web服务器
- 网络服务和应用的开发，需要优秀的并发处理能力
- 作为学习网络编程和并发处理的高质量参考项目

## 安装与使用
### 先决条件
确保你的系统满足以下条件：
- 支持C++20及以上标准的编译器
- Linux操作系统

### 构建步骤
```bash
git clone ...
mkdir build
cd build
cmake ..
make
```
### 运行示例
运行内置的HTTP服务器示例：

```bash
./server
```
## 性能测试
在8核16GB RAM的PC环境下，使用WebBench进行性能测试，ReactorNetX支持上万级别的并发处理，达到了15.4万QPS的查询处理速度。

## 贡献
我们欢迎任何形式的贡献，无论是新特性，bug修复，还是性能优化。请fork此仓库并提交Pull Requests。

## 许可证
该项目采用MIT许可证。详细信息请参阅LICENSE。