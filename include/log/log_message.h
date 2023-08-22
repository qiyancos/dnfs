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

#include <iostream>
#include <vector>
#include "log.h"

/*日志信息对象，存放了单词输出的日志信息结构*/
class LogMessage {
private:
    /*日志的记录内容的格式*/
    char log_message[200]{};

    /*模块名，用来查询默认的日志格式*/
    std::string module_name;

    /*调用的文件完成路径*/
    std::string file_path;

    /*调用行号*/
    int line_no;

    /*调用方法名*/
    std::string func_name;

    /*记录时间戳,用来同一个文件写入时排序*/
    time_t record_time;

    /*记录日志级别*/
    LogLevel log_level = NOLOG;
public:
    /*初始化参数
     * params module_name:模块名
     * params log_level:日志级别
     * params file:调用文件完整路径
     * params line:调用行号
     * params func:调用方法名
     * params format:用户信息日志格式
     * params args:用户信息参数，需对应format
     * */
    LogMessage(const std::string &module_name,
               const LogLevel &log_level,
               const std::string &file, const int &line,
               const std::string &func, const char *format, va_list args);

    /*获取日志记录时间*/
    static std::string get_record_time(const time_t &timeStamp);
};

#endif //LOG_LOG_MESSAGE_H
