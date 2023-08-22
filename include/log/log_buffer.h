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

#ifndef LOG_LOG_BUFFER_H
#define LOG_LOG_BUFFER_H

#include <iostream>
#include <map>
#include <atomic>

#include "log_message.h"

/*日志输出Buffer，单独线程处理，需要对多线程做多队列*/
class LogBuffer {
private:
    /*原子数据，记录缓存数据数量*/
    static std::atomic<int> log_num;

    /*保存每个线程日志缓存列表 线程标识 日志信息列表*/
    std::map<std::string, std::vector<LogMessage>> buffer_map;

    /*保存文件句柄的字典 文件路径 文件句柄*/
    std::map<std::string, FILE *> file_handles;

    /*缓存最大限制，超出限制则将缓存落盘*/
    int buffer_limit = 100;
private:
    /*无参构造函数，使用默认的缓存限制*/
    LogBuffer();

public:
    /*将缓存写入文件,监听log_num*/
    void output_thread();

    /*得到日志缓存单例对象*/
    static LogBuffer &get_instance();

    /*设置缓存限制
     * params b_limit:设置的缓存限制
     * */
    void set_limit(const int &b_limit);

    /*添加线程对应的日志，每次记录log_num递增
     * params thread_name:线程名
     * params log_message:打印信息保存对象
     * */
    void add_log_buffer(const std::string &thread_name, const LogMessage &log_message);
};

/*设置全局log_buffer实例*/
extern LogBuffer log_buffer;

#endif //LOG_LOG_BUFFER_H
