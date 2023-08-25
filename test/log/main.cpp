#include <iostream>
#include "log/log.h"
#include "utils/common_utils.h"
#include <cstdarg>
#include <unistd.h>
#include <experimental/filesystem>

using namespace std;

void exit_t(int code) {
    cout << "错误退出" << endl;
    exit(code);
}

void test(const char *format, ...) {
    char wd[12];
    va_list arguments;

    va_start(arguments, format);

    int s = vsnprintf(wd, 12, format, arguments);
    cout << s << endl;
    cout << wd << endl;

    va_end(arguments);
}

int main() {
    /*建立退出函数*/
    logger.set_exit_func(3, exit_t);

    /*设置全局配置*/
    logger.init("xx", "xx");

    /*解析日志级别*/
    cout << "解析日志级别:" << Logger::decode_log_level("DEBUG_INFO")
         << endl;

    /*建立模板*/
    string w;
    string *s = &w;
    logger.init_module("test");
    logger.init_module("yes");

    /*设置默认模板*/
    cout << "设置默认模板:" << logger.set_default_attr_from("test", s)
         << ":" << *s << endl;

    /*复制日志模板*/
    cout << "复制日志模板:"
         << logger.copy_module_attr_from("yes", "test", s) << ":" << *s << endl;

    /*设置所有模块日志等级日志文件配置*/
    cout << "设置所有模块日志等级日志文件配置:" << logger.set_log_output(
            "stderr:syslog:/tmp/@(time,MINUTE,30):/tmp/dasdad/", s) << ":" << *s
         << endl;

    /*设置所有模块多个日志等级日志文件配置*/
    /*单个模式更改*/
    vector<log_level_t> log_level_list = {EXIT_ERROR};
    cout << "设置所有模块多个日志等级日志文件配置:"
         << logger.set_log_output(log_level_list,
                                  "stderr:syslog:/tmp/yes/@(time,MINUTE,30):/tmp/dasdad/",
                                  s) << ":" << *s << endl;

    /*设置指定日志等级日志文件配置*/
    /*设置全模式更改*/
    cout << "设置所有模块指定日志等级日志文件配置:"
         << logger.set_log_output(L_ERROR,
                                  "stderr:syslog",
                                  s) << ":" << *s << endl;

    /*设置指定模块日志等级日志文件配置*/
    cout << "设置指定模块日志等级日志文件配置:"
         << logger.set_module_log_output("yes",
                                         "stderr:syslog:/tmp/test/@(time,MINUTE,30):/tmp/dasdad/",
                                         s) << ":" << *s << endl;

    /*设置指定日志等级日志文件配置*/
    cout << "设置指定模块单个日志等级日志文件配置:"
         << logger.set_module_log_output("yes",
                                         L_ERROR,
                                         "stderr:syslog:/tmp/single/@(time,MINUTE,30):/tmp/dasdad/",
                                         s) << ":" << *s << endl;

    /*设置多个日志等级日志文件配置*/
    cout << "设置指定模块多个日志等级日志文件配置:"
         << logger.set_module_log_output("yes",
                                         log_level_list,
                                         "stderr:syslog:/tmp/no/@(time,MINUTE,30):/tmp/dasdad",
                                         s) << ":" << *s << endl;

    /*设置所有模块的日志等级，高于该等级的才可以输出*/
    cout << "设置所有模块的日志等级，高于该等级的才可以输出:" << endl;
    logger.set_log_level(L_INFO);

    /*设置指定模块日志等级，高于该等级的才可以输出*/
    cout << "设置指定模块日志等级，高于该等级的才可以输出:"
         << logger.set_module_log_level("yes", D_INFO, s) << ":" << endl;

    /*设置所有模块日志格式*/
    cout << "设置所有模块日志格式:"
         << logger.set_formatter("%(program_name)-%(process)-%(asctime):%(message)", s)
         << ":"
         << *s << endl;

    /*设置指定模块日志格式*/
    cout << "设置指定模块日志格式:" << logger.set_module_formatter("yes",
                                                                   "%(program_name)-%(modulename)-%(asctime):%(message)",
                                                                   s) << ":"
         << *s << endl;

    /*判断模块日志debug状态*/
    cout << "判断模块日志debug状态:" << logger.is_module_debug_on("yes")
         << endl;

    /*设置所有模块日志格式*/
    cout << "设置所有模块日志格式:"
         <<endl;
    logger.set_data_format("");

    /*设置单独模块日志格式*/
    cout << "设置单独模块日志格式:"
         << logger.set_module_data_format("yes", "%Y-%m-%d %H:%M:%S", s) << ":"
         <<*s<< endl;

    /*获取日期*/
    cout << get_record_time(time(nullptr), "") << endl;

    /*格式化模块日志*/
    string message;
    cout << "格式化模块日志:"
         << logger.format_module_log("yes", message, L_ERROR, __FILE__,
                                     __LINE__, __func__, "main.cpp",
                                     time(nullptr), this_thread::get_id(),
                                     getpid(), "what fuck", s) << ":" << *s<<":"
         <<"message:"<<message<< endl;

    /*将所有的模板设置为默认属性*/
    cout << "将所有的模板设置为默认属性:"<< endl;
    logger.set_all_module_attr_default();

    LOG("yes", L_ERROR, "%s", "what fuck");
    LOG("test", D_ERROR, "%s", "what fuck");
    LOG("test", EXIT_ERROR, "%s", "what fuck");
    return 0;
}


