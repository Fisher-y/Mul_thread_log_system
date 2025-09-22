// log_queue.h
#pragma once
#include <mutex>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <chrono>

class DoubleBufferedLogQueue {
public:
    DoubleBufferedLogQueue(size_t buffer_size = 1024);
    void push(const std::string& msg);
    bool pop(std::vector<std::string>& batch);
    void shutdown();
private:
    void trySwapBuffers();
    
    std::vector<std::string> frontBuffer_;  // 生产者写入
    std::vector<std::string> backBuffer_;   // 消费者读取
    
    std::mutex writeMutex_, swapMutex_;
    std::condition_variable cond_var_;
    std::atomic<bool> is_shutdown_{false};
    const size_t maxBufferSize_;

    std::chrono::steady_clock::time_point lastSwapTime_; // 记录上次交换时间
};
