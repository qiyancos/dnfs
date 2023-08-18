#include <iostream>
#include "log/log.h"
#include <time.h>

void exit_t(int code) {
    std::cout << "错误退出" << std::endl;
    exit(code);
}

int main() {
    /*建立退出函数*/
    Logger::set_exit_func(3, exit_t);

    /*设置全局属性*/
    Logger::init("xx", "xx");

    /*解析日志类别*/
    std::cout << Logger::decode_log_level("DEBUG_INFO") << std::endl;

    /*测试建立模板和设置默认模板*/
    std::string w;
    std::string *s = &w;
    logger.init_module("test");
    std::cout << Logger::set_default_attr_from("test", s) << std::endl;
    std::cout << *s << std::endl;

    /*赋值模块日志模板*/
    std::cout << logger.copy_module_attr_from("yes", "test", s) << std::endl;
    std::cout << *s << std::endl;
    return 0;
}
