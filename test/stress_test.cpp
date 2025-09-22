#include "../include_/logger.h"
#include "../include_/log_level.h"
#include <iostream>
#include <thread>
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
void run_stressTest() {
    try {
        Logger logger("stress_test.log", LogLevel::TRACE);
        
        const int THREADS = 30;
        const int LOGS_PER_THREAD = 4000;
        
        std::cout << "Starting stress test (" << THREADS << " threads × " 
                  << LOGS_PER_THREAD << " logs)..." << std::endl;
        
        stressTest(logger, THREADS, LOGS_PER_THREAD);
        
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "Stress test completed. Check 'stress_test.log' for output." << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Stress test failed: " << ex.what() << std::endl;
       
    }
}