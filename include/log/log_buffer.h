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
#include <mutex>
#include <condition_variable>
#include <algorithm>

#include "log_message.h"

#define WAIT_TIME 1
#define MAX_MESSAGE_BUFFER_COUNT 100
#define MAX_SINGLE_MESSAGE_COUNT 20


/*日志输出Buffer，单独线程处理，需要对多线程做多队列*/
class LogBuffer {
private:
    /*原子数据，记录缓存数据数量*/
    std::atomic<int> log_num=0;

    /*最长信息队列数目量*/
    std::atomic<size_t> single_vector_size=0;

    /*保存每个线程日志缓存列表 线程标识 日志信息列表*/
    std::map<unsigned int, std::vector<LogMessage>> buffer_map;

    /*设置线程id哈希锁*/
    std::mutex mtx[10];

    /*设置条件锁通知缓存写文件线程*/
    std::condition_variable cond;

    /*通知写线程锁*/
    std::mutex write_mtx;

    /*flush通知锁*/
    std::mutex flush_mtx;

    /*停止buffer线程标志*/
    bool stop_buffer= false;

    /*日志信息存储列表*/
    std::vector<LogMessage> log_massage_list;

public:
    /*输入锁，锁住整个输入buffer*/
    std::mutex buffer_mtx;

public:
    /*将缓存写入文件,监听log_num
     * return
     * */
    void output_thread();

    /*无参构造函数，使用默认的缓存限制*/
    LogBuffer();

    /*添加线程对应的日志，每次记录log_num递增
     * params thread_id:线程id
     * params log_message:打印信息保存对象
     * return
     * */
    void add_log_buffer(const unsigned int &thread_id,
                        const LogMessage &log_message);

    /*更新属性前强制flush
     * return
     * */
    void flush();

    /*生成数据
     * return
     * */
    void out();

    /*设置buffer结束标志
     * return
     * */
    void set_stop_buffer_flag();
};

#endif //LOG_LOG_BUFFER_H
