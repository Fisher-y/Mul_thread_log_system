#include "../include_/log_queue.h"

    LogQueue::LogQueue() :is_shutdown_(false) {}
    // 生产者添加日志
    void LogQueue::push(const std::string& msg){
        std::lock_guard<std::mutex> lock(mutex_);
        if (is_shutdown_) {
            // 队列已关闭，不再接受新日志
            return;
        }
        queue_.push(msg);
        
        cond_var_.notify_one();
 
    } 
     // 消费者取出日志
    bool LogQueue::pop(std::string& msg){
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
    void LogQueue::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    is_shutdown_ = true;
    cond_var_.notify_all();
}                
