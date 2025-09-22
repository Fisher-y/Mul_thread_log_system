// log_queue.h
#pragma once
#include <mutex>
#include <queue>
#include <condition_variable>

class LogQueue {
public:
    LogQueue();
    void push(const std::string& msg);
    bool pop(std::string& msg);
    void shutdown();
private:
    std::queue<std::string> queue_;
    std::mutex mutex_;
    std::condition_variable cond_var_;
    bool is_shutdown_ = false;
};
