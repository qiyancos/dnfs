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
    string s;
    logger.init_module("test");
    logger.init_module("yes");

    /*复制日志模板*/
    cout << "复制日志模板:"
         << logger.copy_module_attr_from("yes", "test", &s) << ":" << s << endl;

    /*设置所有模块日志等级日志文件配置*/
    cout << "设置所有模块日志等级日志文件配置:" << logger.set_log_output(
            "stderr:syslog:/home/jy/Public/log/@(time,MINUTE,30):/home/jy/Public/log/dasdad/", &s) << ":" << s
         << endl;
    LOG("yes", L_ERROR, "%s", "what fuck1");

    /*设置所有模块多个日志等级日志文件配置*/
    /*单个模式更改*/
    vector<log_level_t> log_level_list = {EXIT_ERROR};
    cout << "设置所有模块多个日志等级日志文件配置:"
         << logger.set_log_output(log_level_list,
                                  "stderr:syslog:/home/jy/Public/log/yes/@(time,MINUTE,30):/home/jy/Public/log/dasdad/",
                                  &s) << ":" << s << endl;
    LOG("yes", L_ERROR, "%s", "what fuck2");

    /*设置指定日志等级日志文件配置*/
    /*设置全模式更改*/
    cout << "设置所有模块指定日志等级日志文件配置:"
         << logger.set_log_output(L_ERROR,
                                  "stderr:syslog:/home/jy/Public/log/text",
                                  &s) << ":" << s << endl;
    LOG("yes", L_ERROR, "%s", "what fuck3");

    /*设置指定模块日志等级日志文件配置*/
    cout << "设置指定模块日志等级日志文件配置:"
         << logger.set_module_log_output("yes",
                                         "stderr:syslog:stdout:/home/jy/Public/log/test/@(time,MINUTE,30):/home/jy/Public/log/dasdad/",
                                         &s) << ":" << s << endl;
    LOG("yes", L_ERROR, "%s", "what fuck4");

    /*设置指定日志等级日志文件配置*/
    cout << "设置指定模块单个日志等级日志文件配置:"
         << logger.set_module_log_output("yes",
                                         L_INFO,
                                         "stderr:syslog:/home/jy/Public/log/single/@(time,MINUTE,30):/home/jy/Public/log/dasdad/",
                                         &s) << ":" << s << endl;
    LOG("yes", L_ERROR, "%s", "what fuck5");

    /*设置多个日志等级日志文件配置*/
    cout << "设置指定模块多个日志等级日志文件配置:"
         << logger.set_module_log_output("yes",
                                         log_level_list,
                                         "stderr:syslog:/home/jy/Public/log/no/@(size,3gb,30):/home/jy/Public/log/dasdad",
                                         &s) << ":" << s << endl;
    LOG("yes", L_ERROR, "%s", "what fuck6");

    /*设置所有模块的日志等级，高于该等级的才可以输出*/
    cout << "设置所有模块的日志等级，高于该等级的才可以输出:" << endl;
    logger.set_log_level(L_INFO);

    /*设置指定模块日志等级，高于该等级的才可以输出*/
    cout << "设置指定模块日志等级，高于该等级的才可以输出:"
         << logger.set_module_log_level("yes", D_INFO, &s) << ":" << endl;

    /*设置所有模块日志格式*/
    cout << "设置所有模块日志格式:"
         << logger.set_formatter("%(asctime):%(message)", &s)
         << ":"
         << s << endl;
    LOG("yes", L_ERROR, "%s", "what fuck7");

    /*设置指定模块日志格式*/
    cout << "设置指定模块日志格式:" << logger.set_module_formatter("yes",
                                                                   "%(modulename)-%(levelname)-%(asctime):%(message)",
                                                                   &s) << ":"
         << s << endl;
    LOG("yes", L_ERROR, "%s", "what fuck8");

    /*判断模块日志debug状态*/
    cout << "判断模块日志debug状态:" << logger.is_module_debug_on("yes")
         << endl;

    /*设置所有模块日志格式*/
    cout << "设置所有模块日志格式:"
         <<endl;
    logger.set_date_format("");
    LOG("yes", L_ERROR, "%s", "what fuck9");


    /*设置单独模块日志格式*/
    cout << "设置单独模块日志格式:"
         << logger.set_module_date_format("yes", "%Y-%m-%d %H:%M:%S", &s) << ":"
         <<s<< endl;
    LOG("yes", L_ERROR, "%s", "what fuck10");

    /*设置默认模板*/
    cout << "设置默认模板:" << logger.set_default_attr_from("yes", &s)
         << ":" << s << endl;

    /*将所有的模板设置为默认属性*/
    cout << "将所有的模板设置为默认属性:"<< endl;
    logger.set_all_module_attr_default();
    LOG("yes", L_ERROR, "%s", "what fuck11");
    cout << "--------" << endl;
//    this_thread::sleep_for(chrono::seconds(2));
    LOG("test", L_ERROR, "%s", "what fuck12");
//    this_thread::sleep_for(chrono::seconds(2));
    LOG("test", L_ERROR, "%s", "what fuck13");
    while(true);

    return 0;
}


