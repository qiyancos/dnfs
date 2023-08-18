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

/*日志信息对象，存放了单词输出的日志信息结构*/
class LogMessage {
private:
    /*日志的记录内容的格式*/
    std::string formatter;
    /*匹配formatter的参数列表*/
    std::vector<std::string> args;
    /*模块名，用来查询默认的日志格式*/
    std::string module_name;
    /*记录时间戳,用来同一个文件写入时排序*/
    int record_time;
public:
    /*初始化参数*/
    LogMessage(std::string &formatter, std::string module_name, std::vector<std::string> args);

    /*解析数据得到日志写入内容*/
    std::string message();

    /*获取日志记录时间*/
    static std::string get_record_time(const time_t &timeStamp);
};

#endif //LOG_LOG_MESSAGE_H
