# 多线程日志系统项目介绍

## 项目概述

这是一个基于C++11/14的高性能多线程日志系统，通过双缓冲队列和异步写入机制，实现了在高效记录日志的同时，最大限度地减少对主线程性能的影响，适合需要高吞吐量的应用场景。
## 核心设计目标：
- 高性能: 采用异步操作和批量写入策略，减少 I/O 阻塞。
- 线程安全: 确保多线程环境下日志记录的安全性和一致性。
- 灵活性: 支持多种日志级别和格式化输出。
- 易用性: 提供简洁的 API 接口。
  
## 功能特性
- 多日志级别支持: 提供 TRACE, DEBUG, INFO, WARNING, ERROR, FATAL 六个日志级别，可根据需要过滤不同重要程度的日志信息。
- 异步日志记录: 日志写入操作在后台线程执行，不阻塞主业务线程，显著提升程序性能。
- 双缓冲队列 (Double-Buffering): 实现了生产者和消费者模型，通过两个缓冲区交替使用，一方面减少线程同步开销，另一方面通过批量写入提高 I/O 效率。队列支持双触发机制（缓冲区满或超时）确保日志及时写入。
- 线程安全: 使用互斥锁 (std::mutex) 和条件变量 (std::condition_variable) 保证多线程并发写入的安全性。
- 格式化输出: 支持类似 printf 的格式化语法，使用 formatMessage 函数和可变参数模板进行消息格式化，支持 {} 占位符。
- 自动时间戳: 每条日志自动附加精确的时间戳。
- 可配置的日志级别: 可在运行时动态设置最低日志记录级别。


## 依赖要求
- C++11及以上标准编译器
- Linux/Windows/macOS平台
- CMake 3.10+（可选，用于构建测试）
  
## 使用示例
### 基础用法
```cpp
Logger logger("app.log", LogLevel::DEBUG); // 创建日志记录器，最低级别为DEBUG

// 基本日志记录
logger.log(LogLevel::INFO, "Application started");

// 带参数的日志
int userId = 123;
logger.log(LogLevel::INFO, "User {} logged in", userId);

// 多线程测试
   auto log_task = [&logger](int id) {
         for (int i = 0; i < 5; ++i) {
          logger.log(LogLevel::INFO,"Thread {}: iteration {}", id, i);
           std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        };
        
  std::thread t1(log_task, 1);
        std::thread t2(log_task, 2);
        
        t1.join();
        t2.join();
// 修改日志级别
logger.setLogLevel(LogLevel::WARNING); // 只记录WARNING及以上级别的日志

```

### 结果：

```cpp
 [INFO] Starting application. 2025-09-11 16:28:02
 [INFO] User 42 performed login in 3.5 seconds. 2025-09-11 16:28:02
 [INFO] Hello World 2025-09-11 16:28:02
 [WARNING] This is a message without placeholders. 2025-09-11 16:28:02
 [INFO] Thread 1: iteration 0 2025-09-11 16:28:02
 [INFO] Thread 2: iteration 0 2025-09-11 16:28:02
 [INFO] Thread 1: iteration 1 2025-09-11 16:28:02
 [INFO] Thread 2: iteration 1 2025-09-11 16:28:02
 [INFO] Thread 2: iteration 2 2025-09-11 16:28:02
 [INFO] Thread 1: iteration 2 2025-09-11 16:28:02
 [INFO] Thread 2: iteration 3 2025-09-11 16:28:02
 [INFO] Thread 1: iteration 3 2025-09-11 16:28:02
 [INFO] Thread 2: iteration 4 2025-09-11 16:28:03
 [INFO] Thread 1: iteration 4 2025-09-11 16:28:03
 [INFO] Starting application. 2025-09-11 16:35:10
 [INFO] User 42 performed login in 3.5 seconds. 2025-09-11 16:35:10
 [INFO] Hello World 2025-09-11 16:35:10
 [WARNING] This is a message without placeholders. 2025-09-11 16:35:10
```
## 运行测试
编译后运行生成的可执行文件，根据提示选择测试类型：

- 单元测试: 验证基本功能、格式化、多线程日志记录和级别过滤。
- 压力测试: 模拟高并发场景，测试系统的吞吐量和稳定性。默认配置为 20 个线程，每个线程写入 5000 条日志。
- 运行所有测试: 依次执行单元测试和压力测试。

## 性能优化建议

- 缓冲区大小: 在创建 DoubleBufferedLogQueue 实例时（Logger 构造函数内部），可以通过 buffer_size 参数指定初始缓冲区大小。根据你的日志流量调整此值。更大的缓冲区可以减少交换次数，但可能增加单次写入的延迟。
- 超时触发: 当前设置为 100 毫秒。即使缓冲区未满，超过此时间也会触发交换和写入，确保日志信息不会在内存中停留过久。可根据对实时性的要求进行调整。
- 批量写入: 后台线程会批量处理日志消息，减少磁盘 I/O 次数，这是高性能的关键。
  
## 扩展性

- 可轻松扩展支持网络日志传输
- 可添加日志滚动功能(按大小或时间分割日志文件)
- 可支持多种日志格式(如JSON格式)
- 可添加日志过滤功能

这个日志系统适合需要高性能、线程安全日志记录的中大型C++项目使用。
