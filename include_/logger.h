// logger.h

#pragma once
#include "log_level.h"
#include "log_queue.h"
#include "format_utils.h"
#include <fstream>
#include <atomic>
#include <thread>
#include <mutex>
class Logger{
public:
    Logger(const std::string& filename, LogLevel min_level);
    ~Logger();

    template<typename... Args>
    void log(LogLevel level,const std::string& format, Args&&... args) {
         std::lock_guard<std::mutex> lock(level_mutex_);
    if (level < min_level_) {
        return;  // 低于最小日志级别，不记录
    }
        std::string time_str = Logger::getCurrentTime();
        std::string level_str = levelToString(level);
        std::string formatted_msg =formatMessage(format, std::forward<Args>(args)...);
        log_queue_.push(" [" + level_str + "] " + formatted_msg+" "+time_str );
    }

    static std::string getCurrentTime();
    void setLogLevel(LogLevel level);
private:
    DoubleBufferedLogQueue log_queue_;
    std::thread worker_thread_;
    std::ofstream log_file_;
    std::atomic<bool> exit_flag_;
    LogLevel min_level_;
    std::mutex level_mutex_;
    void processQueue();
};
