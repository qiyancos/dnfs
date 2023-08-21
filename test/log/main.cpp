#include <iostream>
#include "log/log.h"

using namespace std;

void exit_t(int code) {
    cout << "错误退出" << endl;
    exit(code);
}

int main() {
    /*建立退出函数*/
    logger.set_exit_func(3, exit_t);

    /*设置全局属性*/
    logger.init("xx", "xx");

    /*解析日志级别*/
    cout << "测试解析日志级别:" << logger.decode_log_level("DEBUG_INFO")
         << endl;

    /*建立模板*/
    string w;
    string *s = &w;
    logger.init_module("test");

    /*设置默认模板*/
    cout << "测试设置默认模板:" << logger.set_default_attr_from("test", s)
         << ":" << *s << endl;

    /*复制日志模板*/
    cout << "测试复制日志模板:"
         << logger.copy_module_attr_from("yes", "test", s) << ":" << *s << endl;

    /*设置所有模块日志等级日志文件路径*/
    cout << "设置所有模块日志等级日志文件属性:" << logger.set_log_output(
            "stderr:syslog:/tmp/@(time,MINUTE,30):/tmp/dasdad/", s) << ":" << *s
         << endl;
    return 0;
}
