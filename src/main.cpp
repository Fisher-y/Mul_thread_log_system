#include "../test/unit_test.h"
#include "../test/stress_test.h"
#include <iostream>
#include <string>

int main() {
    int choice = 0;
    
    std::cout << "请选择要运行的测试：" << std::endl;
    std::cout << "1. 单元测试" << std::endl;
    std::cout << "2. 压力测试" << std::endl;
    std::cout << "3. 运行所有测试" << std::endl;
    std::cout << "请输入选择 (1-3): ";
    std::cin >> choice;
    switch(choice) {
        case 1:
            runUnitTests();
            break;
        case 2:
            run_stressTest();
            break;
        case 3:
            runUnitTests();
            run_stressTest();
            break;
        default:
            std::cout << "无效的选择！" << std::endl;
            return 1;
    }
    return 0;
   
}