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
    cout << "解析日志级别:" << logger.decode_log_level("DEBUG_INFO")
         << endl;

    /*建立模板*/
    string w;
    string *s = &w;
    logger.init_module("test");

    /*设置默认模板*/
    cout << "设置默认模板:" << logger.set_default_attr_from("test", s)
         << ":" << *s << endl;

    /*复制日志模板*/
    cout << "复制日志模板:"
         << logger.copy_module_attr_from("yes", "test", s) << ":" << *s << endl;

    /*设置所有模块日志等级日志文件路径*/
    cout << "设置所有模块日志等级日志文件属性:" << logger.set_log_output(
            "stderr:syslog:/tmp/@(time,MINUTE,30):/tmp/dasdad/", s) << ":" << *s
         << endl;

    /*设置所有模块多个日志等级日志文件路径*/
    /*单个模式更改*/
    vector<log_level_t> log_level_list = {EXIT_ERROR};
    cout << "设置所有模块多个日志等级日志文件路径:"
         << logger.set_log_output(log_level_list,
                                  "stderr:syslog:/tmp/yes/@(time,MINUTE,30):/tmp/dasdad/",
                                  s) << ":" << *s << endl;

    /*设置指定日志等级日志文件路径*/
    /*设置全模式更改*/
    cout << "设置指定日志等级日志文件路径:"
         << logger.set_log_output(L_ERROR,
                                  "stderr:syslog:/tmp/all/@(time,MINUTE,30):/tmp/dasdad/",
                                  s) << ":" << *s << endl;

    /*设置指定模块日志等级日志文件属性*/
    cout << "设置指定模块日志等级日志文件属性:"
         << logger.set_module_log_output("yes",
                                         "stderr:syslog:/tmp/test/@(time,MINUTE,30):/tmp/dasdad/",
                                         s) << ":" << *s << endl;

    /*设置指定日志等级日志文件路径*/
    cout << "设置指定日志等级日志文件路径:"
         << logger.set_module_log_output("yes",
                                         L_ERROR,
                                         "stderr:syslog:/tmp/single/@(time,MINUTE,30):/tmp/dasdad/",
                                         s) << ":" << *s << endl;

    /*设置多个日志等级日志文件路径*/
    cout << "设置多个日志等级日志文件路径:"
         << logger.set_module_log_output("yes",
                                         log_level_list,
                                         "stderr:syslog:/tmp/no/@(time,MINUTE,30):/tmp/dasdad/",
                                         s) << ":" << *s << endl;

    /*设置所有模块的日志等级，高于该等级的才可以输出*/
    cout << "设置所有模块的日志等级，高于该等级的才可以输出:" << endl;
    logger.set_log_level(L_INFO);

    /*设置指定模块日志等级，高于该等级的才可以输出*/
    cout << "设置指定模块日志等级，高于该等级的才可以输出:" << endl;
    logger.set_module_log_level("no", D_INFO);

    /*设置所有模块日志格式*/
    cout << "设置所有模块日志格式:" << logger.set_formatter(" %(program_name)/%(process)", s) << ":"
         << *s << endl;

    /*设置所有模块日志格式*/
    cout << "设置所有模块日志格式:" << logger.set_module_formatter("yes"," %(program_name)/%(process)/%(modulename)", s) << ":"
         << *s << endl;

    /*判断模块日志debug状态*/
    cout << "判断模块日志debug状态:" << logger.is_module_debug_on("yes")<<endl;

    return 0;
}
