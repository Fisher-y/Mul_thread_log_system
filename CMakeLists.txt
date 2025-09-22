cmake_minimum_required(VERSION 3.12)
project(LoggerSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

# 添加主日志库
add_library(logger_system
    src/log_level.cpp
    src/log_queue.cpp
    src/logger.cpp
    src/main.cpp
)
target_include_directories(logger_system PUBLIC include)
target_link_libraries(logger_system PUBLIC Threads::Threads)

# 添加测试实现库（重要：将测试实现编译成静态库）
add_library(test_implementations STATIC
    test/unit_test.cpp
    test/stress_test.cpp
)
target_include_directories(test_implementations PUBLIC test) # 暴露tests目录以便main.cpp找到头文件
target_link_libraries(test_implementations PUBLIC logger_system) # 链接到主日志库

# 添加主可执行文件
add_executable(logger_app src/main.cpp)
# 主程序需要链接日志库和测试实现库
target_link_libraries(logger_app PRIVATE logger_system test_implementations)
