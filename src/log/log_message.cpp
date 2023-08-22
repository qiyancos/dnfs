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

#include "log/log_message.h"

using namespace std;

/*将时间戳转化为字符串*/
string LogMessage::get_record_time(const time_t &timeStamp) {

    /*保存转化时间*/
    char time_buffer[30] = {0};

    /*构建时间存储结构*/
    struct tm *info;

    /*将时间戳转化为为时间存储结构*/
    info = localtime(&timeStamp);

    /*转化时间格式*/
    strftime(time_buffer, 30, "%Y-%m-%d %H:%M:%S", info);

    return time_buffer;
}

/*初始化参数
 * params module_name:模块名
 * params log_level:日志级别
 * params file:调用文件完整路径
 * params line:调用行号
 * params func:调用方法名
 * params format:用户信息日志格式
 * params args:用户信息参数，需对应format
 * */
LogMessage::LogMessage(const std::string &module_name,
                       const LogLevel &log_level,
                       const std::string &file, const int &line,
                       const std::string &func, const char *format,
                       va_list args) {
    /*格式化字符串*/
    vsnprintf(log_message, 200, format, args);

    /*参数赋值*/
    this->module_name = module_name;
    this->log_level = log_level;
    file_path = file;
    line_no = line;
    func_name = func;
    record_time = time(nullptr);
}
