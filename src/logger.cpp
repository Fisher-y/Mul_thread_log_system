#include "../include_/logger.h"

#include <chrono>
#include <iomanip>
#include <stdexcept>  

    std::string Logger::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") ;
     return oss.str();
}

//Logger类

    Logger::Logger(const std::string& filename,LogLevel min_level) 
        : log_file_(filename, std::ios::out | std::ios::app), 
          exit_flag_(false),
          min_level_(min_level),
          worker_thread_(&Logger::processQueue, this){
        if (!log_file_.is_open()) {
            throw std::runtime_error("无法打开日志文件");
        }
    }

    Logger::~Logger() {
         exit_flag_ = true; 
        log_queue_.shutdown();
        if (worker_thread_.joinable()) {
            worker_thread_.join();
        }
        if (log_file_.is_open()) {
            log_file_.close();
        }
    }


     void Logger::processQueue() {
       std::vector<std::string> batch;
         if (log_queue_.pop(batch)) {  // 批量获取日志
            for (const auto& msg : batch) {
                log_file_ << msg << "\n";  // 批量写入
            }
        }
        log_file_.flush();
    }



     void Logger::setLogLevel(LogLevel level) {
     std::lock_guard<std::mutex> lock(level_mutex_);
     min_level_ = level;
    }


    
   
