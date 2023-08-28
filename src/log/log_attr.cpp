/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Jiao Yue 3059497228@qq.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License; This program is
 * distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the MIT lisence for
 * more details. You should have received a copy of the MIT License
 * along with this project.
 *
 */
#include "log/log_attr.h"
#include "log/log.h"
#include "utils/common_utils.h"
#include "utils/thread_utils.h"
#include <regex>

using namespace std;

/*模块日志属性默认构造函数*/
LoggerAttr::LoggerAttr() {
    /*message默认打开*/
    log_formatter_select[FMT_LOG_MESSAGE] = true;
}

/*根据格式字符串，建立日志格式,供设置日志格式调用
 * params error_info:错误信息
 * return: 状态码 0 生成成功 其他 生成失败
 * */
int
LoggerAttr::init_log_formatter(string *error_info) {
    /*用来判定是否设置了格式，没有设置至少一个格式报错*/
    bool set_formatter = false;
    int i=0;
    /*循环判定是否有关键字*/
    for (auto &log_formate: log_formatter_dict) {
        /*如果查找到了关键字*/
        if (formatter.find(log_formate.second.first) != string::npos) {
            /*设置字段选中*/
            log_formatter_select[i]=true;
            /*设置了格式*/
            set_formatter = true;
        } else {
            log_formatter_select[i]=false;
        }
        i+=1;
    }
    /*如果没有设置格式*/
    if (!set_formatter) {
        SET_PTR_INFO(error_info, format_message(
                "The log format setting must be selected from the list below:\n"
                "        * %(program_name) the program name\n"
                "        * %(hostname) the host name\n"
                "        * %(levelname) log level\n"
                "        * %(pathname) the complete path for the module what use log\n"
                "        * %(filename) the file name what for the module what use log\n"
                "        * %(modulename) the module name\n"
                "        * %(funcName) the function name for the module what use log\n"
                "        * %(lineno) the line number for the module what use log\n"
                "        * %(created) now time (UNIX float)\n"
                "        * %(relativeCreated) the ms from log build\n"
                "        * %(asctime) the time formatter default(2023-08-18 11:18:45998)\n"
                "        * %(thread) the thread id\n"
                "        * %(threadName) the thread name\n"
                "        * %(process) the progress id\n"
                "        * %(message) the log message\n"
                "you set is %s",formatter.c_str())
        )
        return 1;
    }
    return 0;
}

/*根据formatter得到日志信息
 * params log_message:根据设置的formatter生成的日志消息
 * params log_le:输出的日志级别
 * params file:调用文件完整路径
 * params line:调用行号
 * params func:调用方法名
 * params file_name:调用文件名
 * params record_time:创建时间
 * params tid:线程id
 * params pid:进程id
 * params message:用户打印的消息
 * return
 * */
void
LoggerAttr::get_log_message(string &log_message, log_level_t log_le,
                            const string &file,
                            const int &line, const string &func,
                            const string &file_name,
                            const time_t &record_time,
                            const thread::id &tid,
                            const int &pid,
                            const string & message) {
    /*获取设置的日志格式进行判断替换*/
    log_message = formatter;

    /*循环判定选中的格式，选中就替换*/
    for (unsigned int i = 0; i < FMT_LOG_FORMAT_COUNT; i++) {
        /*如果选择了*/
        if (log_formatter_select[i]) {
            /*直接判定进行正则替换*/
            switch (i) {
                case 0:
                    /*项目名称*/
                    log_message = regex_replace(log_message,
                                                std::regex(
                                                        "%\\(program_name\\)"),
                                                logger.program_name);
                case 1:
                    /*主机名*/
                    log_message = regex_replace(log_message,
                                                std::regex("%\\(hostname\\)"),
                                                logger.program_name);
                case 2:
                    /*日志级别数字*/
                    log_message = regex_replace(log_message,
                                                std::regex("%\\(levelname\\)"),
                                                log_level_info_dict[log_le].first[0]);
                case 3:
                    /*输出模块的完整路径名*/
                    log_message = regex_replace(log_message,
                                                std::regex("%\\(pathname\\)"),
                                                file);
                case 4:
                    /*输出模块的文件名*/
                    log_message = regex_replace(log_message,
                                                std::regex("%\\(filename\\)"),
                                                file_name);
                case 5:
                    /*模块名*/
                    log_message = regex_replace(log_message,
                                                std::regex("%\\(modulename\\)"),
                                                module_name);
                case 6:
                    /*调用方法名*/
                    log_message = regex_replace(log_message,
                                                std::regex("%\\(funcName\\)"),
                                                func);
                case 7:
                    /*调用行号*/
                    log_message = regex_replace(log_message,
                                                std::regex("%\\(lineno\\)"),
                                                to_string(line));
                case 8:
                    /*当前时间，UNIX浮点数表示*/
                    log_message = regex_replace(log_message,
                                                std::regex("%\\(created\\)"),
                                                to_string(record_time));
                case 9:
                    /*自logger创建以来的毫秒数*/
                    log_message = regex_replace(log_message,
                                                std::regex(
                                                        "%\\(relativeCreated\\)"),
                                                to_string(logger.init_time -
                                                          time(nullptr)));
                case 10:
                    /* 字符串形式的当前时间 默认为2023-08-18 11:18:45998*/
                    log_message = regex_replace(log_message,
                                                std::regex("%\\(asctime\\)"),
                                                get_record_time(record_time,
                                                                date_format));
                case 11:
                    /*打印线程id*/
                    /*将pid转化为字符串*/
                    log_message = regex_replace(log_message,
                                                std::regex("%\\(thread\\)"),
                                                pid_to_string(tid));
                case 12:
                    /*打印线程名*/
                    log_message = regex_replace(log_message,
                                                std::regex("%\\(threadName\\)"),
                                                ThreadPool::get_target_thread_name(
                                                        tid));
                case 13:
                    /*打印进程id*/
                    log_message = regex_replace(log_message,
                                                std::regex("%\\(process\\)"),
                                                to_string(pid));
                case 14:
                    /*打印用户信息*/
                    log_message = regex_replace(log_message,
                                                std::regex("%\\(message\\)"),
                                                message);
                default:
                    continue;
            }
        }

    }
}


/*判断debug模式
 * params log_attr:需要判定的日志属性结构体对象，直接更改其属性
 * params log_level:判定的日志等级
 * */
void LoggerAttr::judge_debug() {
    /*遍历别名列表*/
    for (const string &log_l: log_level_info_dict[log_level].first)
        /*进行debug模式判定,如果包含DEBUG,设置为true*/
        if (log_l.find("DEBUG") !=
            string::npos) {
            debug_on = true;
        } else {
            /*清空之前的设置*/
            debug_on = false;
        }
}
