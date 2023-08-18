#include <iostream>
#include "log/log.h"
using namespace std;
void exit_t(int code) {
    cout << "错误退出" << endl;
    exit(code);
}

int main() {
    /*建立退出函数*/
    Logger::set_exit_func(3, exit_t);

    /*设置全局属性*/
    Logger::init("xx", "xx");

    int in_s=Logger::decode_log_level("DEBUG_INFO");

    /*解析日志类别*/
    cout << in_s << endl;

    /*测试建立模板和设置默认模板*/
    string w;
    string *s = &w;
    logger.init_module("test");
    cout << Logger::set_default_attr_from("test", s) << endl;
    cout << *s << endl;

    /*赋值模块日志模板*/
    cout << logger.copy_module_attr_from("yes", "test", s) << endl;
    cout << *s << endl;
    return 0;
}
