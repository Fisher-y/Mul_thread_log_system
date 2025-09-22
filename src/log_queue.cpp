//log_queue.cpp
#include "../include_/log_queue.h"

   DoubleBufferedLogQueue::DoubleBufferedLogQueue(size_t buffer_size) : maxBufferSize_(buffer_size) {}    
    // 生产者添加日志
    void DoubleBufferedLogQueue::push(const std::string& msg){
        std::lock_guard<std::mutex> lock(writeMutex_);
        if (is_shutdown_) {
            // 队列已关闭，不再接受新日志
            return;
        }
        frontBuffer_.push_back(msg);
        
        if (frontBuffer_.size() >= maxBufferSize_) trySwapBuffers();
    } 
     // 消费者取出日志
    bool DoubleBufferedLogQueue::pop(std::vector<std::string>& batch){
         std::unique_lock<std::mutex> lock(swapMutex_);
          if (backBuffer_.empty() && !cond_var_.wait_for(lock, std::chrono::milliseconds(100), 
            [this] { return !backBuffer_.empty() || is_shutdown_; })) {
            return false;
        }
         batch.swap(backBuffer_);
         backBuffer_.clear();
         return true;
    }    
    void DoubleBufferedLogQueue::trySwapBuffers() {
    std::lock_guard<std::mutex> lock(swapMutex_);
    frontBuffer_.swap(backBuffer_);
    frontBuffer_.clear();
    cond_var_.notify_one();
}
      // 关闭队列    
    void DoubleBufferedLogQueue::shutdown() {
    is_shutdown_ = true;
    cond_var_.notify_all();
}                
