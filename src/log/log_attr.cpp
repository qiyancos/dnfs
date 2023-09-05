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
    int i = 0;
    /*循环判定是否有关键字*/
    for (auto &log_formate: log_formatter_dict) {
        /*如果查找到了关键字*/
        if (formatter.find(log_formate.second.first) != string::npos) {
            /*设置字段选中*/
            log_formatter_select[i] = true;
            /*设置了格式*/
            set_formatter = true;
        } else {
            log_formatter_select[i] = false;
        }
        i += 1;
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
                "you set is %s", formatter.c_str())
        )
        return 1;
    }
    return 0;
}

/*判断debug模式
 * return
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

/*获取debug状态
 * return 是否debug模式
 * */
bool LoggerAttr::get_debug() const {
    return debug_on;
}

/*调用输出方法
 * params log_level:输出日志等级
 * params message:日志信息
 * params error_info:错误信息
 * return: 状态码 0 生成成功 其他 生成失败
 * */
int LoggerAttr::out_message(const log_level_t &message_log_level,
                            const string &message,
                            string *error_info) {
    /*调用对应的输出模块*/
    return log_level_output[message_log_level].out_message(message, error_info);
}

/*更改内部log_file模块名
 * params change_module_name:更改的模块名
 * */
void LoggerAttr::set_module_name(const string& change_module_name) {
    /*遍历更改log_file模块名*/
    for (LogOutputAttr &log_output: log_level_output) {
        log_output.set_module_name(change_module_name);
    }
}


