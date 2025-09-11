# Readme

# 多线程日志系统项目介绍

## 项目概述

这是一个基于C++11的多线程日志系统，实现了高效的异步日志记录功能。该系统采用生产者-消费者模型，支持多线程并发写入日志，同时保证线程安全。

## 技术栈

- **C++11标准**：使用了现代C++特性
- **多线程编程**：使用`std::thread`、`std::mutex`和`std::condition_variable`
- **原子操作**：使用`std::atomic`保证标志位的线程安全
- **字符串格式化**：实现类似Python的`format`风格的格式化功能
- **文件I/O**：使用`std::ofstream`进行日志文件写入

## 核心组件

### 1. LogQueue (线程安全日志队列)

- 使用`std::queue`作为底层容器
- 通过`std::mutex`和`std::condition_variable`实现线程安全
- 支持优雅关闭(shutdown)机制
- 实现了生产者-消费者模式

### 2. 格式化工具

- 支持类似Python的`{}`占位符格式化
- 使用可变参数模板处理任意数量和类型的参数
- 自动将各种类型转换为字符串

### 3. Logger类 (核心日志记录器)

- 异步日志记录：日志写入操作在后台线程执行
- 支持多日志级别(TRACE, DEBUG, INFO, WARNING, ERROR, FATAL)
- 可配置的最小日志级别
- 自动时间戳记录
- 线程安全的日志级别设置

## 工作原理

1. **日志产生**：当调用`log()`方法时，日志消息被格式化并放入线程安全的队列中
2. **后台处理**：独立的工作线程从队列中取出日志消息并写入文件
3. **线程同步**：使用条件变量实现高效的通知机制，避免忙等待
4. **优雅关闭**：析构时通知工作线程完成剩余日志写入后退出

## 性能特点

- **非阻塞写入**：主线程不会被文件I/O操作阻塞
- **批量写入**：减少磁盘操作次数
- **线程安全**：多线程同时写入日志不会导致数据竞争
- **可配置性**：可设置日志级别过滤不重要信息

## 使用示例

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

## 结果：

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

## 扩展性

- 可轻松扩展支持网络日志传输
- 可添加日志滚动功能(按大小或时间分割日志文件)
- 可支持多种日志格式(如JSON格式)
- 可添加日志过滤功能

这个日志系统适合需要高性能、线程安全日志记录的中大型C++项目使用。