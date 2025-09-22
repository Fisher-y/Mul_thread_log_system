#include "../include_/logger.h"
#include "../include_/log_level.h"
#include <iostream>
#include <thread>
void runUnitTests() {
    try {
        Logger logger("unit_test.log",LogLevel::DEBUG);
        std::cout<<"Starting unit test"<<std::endl;
        logger.log(LogLevel::INFO, "Starting application.");
        
        int user_id = 42;
        std::string action = "login";
        double duration = 3.5;
        std::string world = "World";
        
        logger.log(LogLevel::INFO, "User {} performed {} in {} seconds.", user_id, action, duration);
        logger.log(LogLevel::INFO, "Hello {}", world);
        logger.log(LogLevel::WARNING, "This is a message without placeholders.");
        logger.log(LogLevel::DEBUG, "Multiple placeholders: {}, {}, {}.", 1, 2, 3);
        
        // 多线程测试
        auto log_task = [&logger](int id) {
            for (int i = 0; i < 5; ++i) {
                logger.log(LogLevel::INFO, "Thread {}: iteration {}", id, i);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        };
        
        std::thread t1(log_task, 1);
        std::thread t2(log_task, 2);
        
        t1.join();
        t2.join();
        
        logger.setLogLevel(LogLevel::WARNING);
        logger.log(LogLevel::INFO, "This INFO message should NOT appear after level change.");
        logger.log(LogLevel::ERROR, "This ERROR message should appear.");
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout<<"Unit test completed.Check 'unit_test.log' for output."<<std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Unit test failed: " << ex.what() << std::endl;
    }
    
}

