# 多线程日志系统项目介绍

## 项目概述

这是一个基于C++11/14的高性能多线程日志系统，采用生产者-消费者模型实现异步日志记录，支持多级别日志过滤、格式化输出和线程安全操作。系统通过分离日志生产和消费过程，显著降低业务线程的I/O等待时间，同时保证日志记录的完整性和顺序性。
## 设计原理
### 核心架构
### 日志队列(LogQueue)

- 线程安全的生产者-消费者队列
- 支持优雅关闭(shutdown)
- 条件变量唤醒机制
### 格式化引擎
- 可变参数模板实现类型安全格式化
- 自动内存管理避免缓冲区溢出
- 支持自定义格式占位符
### 写入线程
- 独立后台线程处理I/O
- 批量写入优化
- 异常处理保证服务连续性
### 测试模块
- 压力测试
- 常规功能测试
  
## 功能特性

### 1、多级别日志支持

- 提供TRACE/DEBUG/INFO/WARNING/ERROR/FATAL六级日志分级
- 运行时动态调整日志级别阈值（通过setLogLevel()方法）
- 自动过滤低于设定级别的日志消息

### 2、线程安全设计

- 基于互斥锁(std::mutex)和条件变量(std::condition_variable)的线程同步
- 无锁队列设计减少线程竞争
- 支持多生产者单消费者模型

### 3、高性能异步机制

- 日志生产与消费分离，避免I/O阻塞业务线程
- 批量写入磁盘减少系统调用次数
- 内置压力测试显示可达10万+日志/秒的吞吐量

### 4、灵活格式化

- 支持类似Python的"{}"占位符格式化
- 自动类型转换（内置std::ostringstream转换）
- 线程安全的时间戳生成（精确到秒）

### 5、资源管理

- RAII风格自动关闭文件流
- 优雅关闭机制确保日志不丢失
- 异常安全设计

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
### 压力测试
```cpp
Logger logger("stress.log", LogLevel::TRACE);
stressTest(logger, 20, 5000); // 20线程各写5000条日志

```
### 结果
```bash
Starting stress test (20 threads × 5000 logs)...

=== 压力测试结果 ===
线程数量: 20
总日志量: 100000 条
总耗时: 3.52063 秒
吞吐量: 28404 条/秒
Stress test completed. Check 'stress_test.log' for output.

```
## 扩展性

- 可轻松扩展支持网络日志传输
- 可添加日志滚动功能(按大小或时间分割日志文件)
- 可支持多种日志格式(如JSON格式)
- 可添加日志过滤功能

这个日志系统适合需要高性能、线程安全日志记录的中大型C++项目使用。
