#include<iostream>
#include<queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include <thread>
#include <vector>
#include <fstream>
#include <atomic>
#include <sstream>
#include <stdexcept>


#include <iomanip>
// 线程安全的日志队列
class LogQueue {
public:
      LogQueue() : is_shutdown_(false) {}
    // 生产者添加日志
    void push(const std::string& msg){
        std::lock_guard<std::mutex> lock(mutex_);
        if (is_shutdown_) {
            // 队列已关闭，不再接受新日志
            return;
        }
        queue_.push(msg);
        
        cond_var_.notify_one();
 
    } 
     // 消费者取出日志
    bool pop(std::string& msg){
         std::unique_lock<std::mutex> lock(mutex_);
         while (queue_.empty() && !is_shutdown_) {
        cond_var_.wait(lock); // 自动释放锁并阻塞
    }
         if (is_shutdown_ && queue_.empty()) return false;
         
         msg = queue_.front();
         queue_.pop();
         return true;
    }    
      // 关闭队列    
    void shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    is_shutdown_ = true;
    cond_var_.notify_all();
}                
private:
    std::queue<std::string> queue_;
    std::mutex mutex_;
    std::condition_variable cond_var_;
    bool is_shutdown_ = false;
};

// 格式化工具（单个参数转字符串）
template<typename T>
std::string to_string_helper(T&& arg) {
    std::ostringstream oss;
    oss << std::forward<T>(arg);
    return oss.str();
}


template<typename... Args>
std::string formatMessage(const std::string& format, Args&&... args) {
    std::vector<std::string> arg_strings = { to_string_helper(std::forward<Args>(args))... };
    std::ostringstream oss;
    size_t arg_index = 0;
    size_t pos = 0;
    size_t placeholder = format.find("{}", pos);


    while (placeholder != std::string::npos) {
        oss << format.substr(pos, placeholder - pos);
        if (arg_index < arg_strings.size()) {
            oss << arg_strings[arg_index++];
        } else {
            oss << "{}";
        }
        pos = placeholder + 2;
        placeholder = format.find("{}", pos);
    }


    oss << format.substr(pos);


    while (arg_index < arg_strings.size()) {
        oss << arg_strings[arg_index++];
    }


    return oss.str();
}

// 在Logger类前添加
enum class LogLevel {
    TRACE,   
    DEBUG,   
    INFO,    
    WARNING,
    ERROR,   
    FATAL    
};

// 在Logger类中添加静态方法
static std::string levelToString(LogLevel level) {
    switch(level) {
        case LogLevel::TRACE:   return "TRACE";
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::FATAL:   return "FATAL";
        default:                return "UNKNOWN";
    }
}


//Logger类
class Logger {
public:
    Logger(const std::string& filename,LogLevel min_level) 
        : log_file_(filename, std::ios::out | std::ios::app), 
          exit_flag_(false),min_level_(min_level){
        if (!log_file_.is_open()) {
            throw std::runtime_error("无法打开日志文件");
        }
        worker_thread_ = std::thread(&Logger::processQueue, this);
    }


    ~Logger() {
        log_queue_.shutdown();
        if (worker_thread_.joinable()) {
            worker_thread_.join();
        }
        if (log_file_.is_open()) {
            log_file_.close();
        }
    }

   static std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    
    
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") ;
     return oss.str();
}

    template<typename... Args>
    void log(LogLevel level,const std::string& format, Args&&... args) {
         std::lock_guard<std::mutex> lock(level_mutex_);
    if (level < min_level_) {
        return;  // 低于最小日志级别，不记录
    }
        std::string time_str = getCurrentTime();
        std::string level_str = levelToString(level);
         std::string formatted_msg =formatMessage(format, std::forward<Args>(args)...);
        log_queue_.push(" [" + level_str + "] " + formatted_msg+" "+time_str );
    }

     void setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(level_mutex_);
    min_level_ = level;
    }


  private:
    LogQueue log_queue_;
    std::thread worker_thread_;
    std::ofstream log_file_;
    std::atomic<bool> exit_flag_;
    LogLevel min_level_;  // 最小日志级别
    std::mutex level_mutex_;  // 保护min_level_的互斥锁

    void processQueue() {
        std::string msg;
        while (log_queue_.pop(msg)) {
            log_file_ << msg << std::endl;
        }
        log_file_.flush();
    }

   
};

void stressTest(Logger& logger, int threadCount, int logsPerThread) {
    std::vector<std::thread> threads;
    auto startTime = std::chrono::high_resolution_clock::now();

    // 创建多个生产者线程
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&logger, logsPerThread, i]() {
            for (int j = 0; j < logsPerThread; ++j) {
                logger.log(LogLevel::INFO, 
                    "Thread[{}] - Message {}: This is a stress test log with values: {}, {:.2f}", 
                    i, j, j * 10, 3.14159 * j);
            }
        });
    }

    // 等待所有线程完成
    for (auto& t : threads) t.join();

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = endTime - startTime;

    // 输出测试结果
    int totalLogs = threadCount * logsPerThread;
    double logsPerSec = totalLogs / duration.count();
    
    std::cout << "\n=== 压力测试结果 ===" << std::endl;
    std::cout << "线程数量: " << threadCount << std::endl;
    std::cout << "总日志量: " << totalLogs << " 条" << std::endl;
    std::cout << "总耗时: " << duration.count() << " 秒" << std::endl;
    std::cout << "吞吐量: " << logsPerSec << " 条/秒" << std::endl;
}


int main(){
  try {
        Logger logger("app.log",LogLevel::DEBUG);
        
        logger.log(LogLevel::INFO,"Starting application.");
        
        int user_id = 42;
        std::string action = "login";
        double duration = 3.5;
        std::string world = "World";
        
        logger.log(LogLevel::INFO,"User {} performed {} in {} seconds.", user_id, action, duration);
        logger.log(LogLevel::INFO,"Hello {}", world);
        logger.log(LogLevel::WARNING,"This is a message without placeholders.");
        logger.log(LogLevel::DEBUG,"Multiple placeholders: {}, {}, {}.", 1, 2, 3);
        
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
        
        // 确保所有日志都被处理
        std::this_thread::sleep_for(std::chrono::seconds(1));

    //   初始化日志系统（设置为最低级别以记录所有日志）
        Logger logger("stress_test.log", LogLevel::TRACE);
        
        // 执行压力测试（参数：线程数，每线程日志量）
        const int THREADS = 20;      // 并发线程数
        const int LOGS_PER_THREAD = 5000; // 每线程写入量
        
        std::cout << "开始压力测试（" << THREADS << "线程 × " 
                  << LOGS_PER_THREAD << "日志）..." << std::endl;
        
        stressTest(logger, THREADS, LOGS_PER_THREAD);
        
        // 等待队列清空
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    catch (const std::exception& ex) {
        std::cerr << "日志系统初始化失败: " << ex.what() << std::endl;
        std::cerr << "测试失败: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}