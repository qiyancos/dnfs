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
#include <string>
#include <unistd.h>
#include <cstdarg>
#include "log/log_message.h"
#include "utils/common_utils.h"
#include "log/log.h"

using namespace std;

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
LogMessage::LogMessage(const string &module_name,
                       const LogLevel &log_level,
                       const string &file, const int &line,
                       const string &func, const char *format,
                       const thread::id &tid,
                       LoggerAttr *log_attr,
                       va_list args) {

    /*建立临时缓存存储数据*/
    char *buffer_message = (char *) malloc(ONE_MB);

    /*复制参数,超出范围重新格式化做准备*/
    va_list buffer;
    va_copy(buffer, args);

    /*格式化字符串*/
    int message_len = vsnprintf(buffer_message, ONE_MB, format, args);

    /*如果添加失败*/
    if (message_len < 0) {
        LOG("logger", L_ERROR,
            "Formatting module %s information failed", module_name.c_str());
    }

    /*判断数据大小*/
    if (message_len >= ONE_MB) {
        /*设置新的指针*/
        char *buffer_message_new;

        if (message_len < MAX_BUFFER) {
            /*动态申请内存*/
            buffer_message_new = (char *) realloc(buffer_message, message_len);
        } else {
            /*动态申请内存*/
            buffer_message_new = (char *) realloc(buffer_message, MAX_BUFFER);

            message_len = MAX_BUFFER;
            LOG("logger", L_WARN,
                "The module %s log information exceeds the size limit, the limit size is %d, the information size is %d",
                module_name.c_str(), MAX_BUFFER, message_len);
        }
        /*如果成功分配空间*/
        if (buffer_message_new) {
            /*重定向结果指针*/
            buffer_message = buffer_message_new;
            /*格式化结果*/
            message_len = vsnprintf(buffer_message, message_len, format, buffer);
            /*如果添加失败*/
            if (message_len < 0) {
                LOG("logger", L_ERROR,
                    "Module %s log message reformatting failed",
                    module_name.c_str());
            }
        } else {
            LOG("logger", L_ERROR,
                "Module %s log information failed to apply for memory",
                module_name.c_str());
        }

    }
    log_message.reset(buffer_message);

    /*参数赋值*/
    this->module_name = module_name;
    this->log_level = log_level;
    this->tid = tid;
    this->log_attr = log_attr;
    file_path = file;
    line_no = line;
    func_name = func;
    record_time = time(nullptr);

    /*切割路径获取文件名*/
    vector<string> path_split;
    split_str(file, "/", path_split);
    file_name = path_split[path_split.size() - 1];

    /*获取进程id*/
    pid = getpid();
}

/*生成日志信息
 * params error_info:错误信息
 * params log_message:生成的日志信息
 * return: 状态码 0 生成成功 其他 生成失败
 * */
int
LogMessage::grnarate_log_message(string &format_message,
                                 std::string *error_info) {

    /*生成消息*/
    log_attr->get_log_message(format_message,
                              log_level,
                              file_path, line_no, func_name, file_name,
                              record_time, tid, pid, log_message.get());
    return 0;
}
