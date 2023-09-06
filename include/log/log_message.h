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

#ifndef LOG_LOG_MESSAGE_H
#define LOG_LOG_MESSAGE_H
#define MAX_BUFFER 65536

#include <iostream>
#include <vector>
#include <thread>
#include <memory>
#include "log_data.h"
#include "log_attr.h"
#include "utils/time_utils.h"


/*日志信息对象，存放了单词输出的日志信息结构*/
class LogMessage {
private:
    /*用户需要打印的日志信息*/
    std::shared_ptr<char> log_message;

    /*模块名，用来查询默认的日志格式*/
    std::string module_name;

    /*调用的文件完整路径*/
    std::string file_path;

    /*调用的文件名*/
    std::string file_name;

    /*调用行号*/
    int line_no;

    /*调用方法名*/
    std::string func_name;

    /*记录日志级别*/
    LogLevel log_level = NOLOG;

    /*线程id*/
    std::thread::id tid;
    /*进程id*/
    pid_t pid;

public:
    /*获取记录时间*/
    Time_T record_time=Time_T();
public:
    /*初始化参数
     * params module_name:模块名
     * params log_level:日志级别
     * params file:调用文件完整路径
     * params line:调用行号
     * params func:调用方法名
     * params format:用户信息日志格式
     * params tid:线程id
     * params args:用户信息参数，需对应format
     * */
    LogMessage(const std::string &module_name,
               const LogLevel &log_level,
               const std::string &file, const int &line,
               const std::string &func, const char *format,
               const std::thread::id &tid,
               va_list args);

    /*生成日志信息
     * params format_message:生成的日志信息
     * return
     * */
    void ganerate_log_message(std::string &format_message);

    /*判断添加调用栈
     * params format_message:生成的日志信息
     * return:
     * */
    void judge_traceback(std::string &format_message);

    /*调用输出方法
     * params message:日志信息
     * return:
     * */
    void out_message(std::string &message);
};

#endif //LOG_LOG_MESSAGE_H
