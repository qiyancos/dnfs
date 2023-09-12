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

#include "log/log_buffer.h"
#include <iostream>

using namespace std;

LogBuffer::LogBuffer() {
    /*初始化buffer map数据*/
    for (int i = 0; i < 10; i++) {
        buffer_map[i] = {};
    }
}

/*将缓存写入文件,监听log_num
 * return
 * */
void LogBuffer::output_thread() {
    while (true) {
        /*自动上锁*/
        unique_lock<mutex> write_uk(write_mtx);
        /*todo加上时间限制*/
        /*等待锁,并且设置延时*/
        cond.wait_for(write_uk, chrono::seconds(WAIT_TIME));

        /*谁有flush锁谁操作*/
        unique_lock<mutex> flush_uk(flush_mtx);
        out();

        /*判断是否结束线程*/
        if (stop_buffer) {
            break;
        }
    }
}

/*添加线程对应的日志，每次记录log_num递增
 * params thread_id:线程id
 * params log_message:打印信息保存对象
 * return
 * */
void LogBuffer::add_log_buffer(const unsigned int &thread_id,
                               const LogMessage &log_message) {

    /*进行哈希计算*/
    unsigned int hash_id = thread_id % 10;

    /*限制锁的范围*/
    {
        /*加入buffer锁*/
        unique_lock<mutex> bk(buffer_mtx);
        /*加入buffer单独队列锁*/
        unique_lock<mutex> uk(mtx[hash_id]);

        /*已经存在进行添加*/
        buffer_map[hash_id].push_back(log_message);
        /*获取最长队列数据量*/
        if (single_vector_size < buffer_map[hash_id].size()) {
            single_vector_size.store(buffer_map[hash_id].size());
        }
    }

    /*加入*/
    /*数据总数加1*/
    log_num += 1;
    unique_lock<mutex> write_uk(write_mtx);
    /*如果数目大于设置的缓存限制*/
    if (log_num > MAX_MESSAGE_BUFFER_COUNT or
        single_vector_size > MAX_SINGLE_MESSAGE_COUNT) {
        cond.notify_one();
    }
}

/*更新属性前强制flush
 * return
 * */
void LogBuffer::flush() {
    /*谁有flush锁谁操作*/
    unique_lock<mutex> flush_uk(flush_mtx);
    out();
}

/*生成数据
 * return
 * */
void LogBuffer::out() {
    /*清空计数器*/
    log_num.store(0);
    /*清空最长信息队列数目量*/
    single_vector_size.store(0);

    /*遍历添加日志信息*/
    for (unsigned int i = 0; i < 10; i++) {
        /*枷锁进行添加删除*/
        unique_lock<mutex> uk(mtx[i]);
        /*追加信息*/
        log_massage_list.insert(log_massage_list.end(),
                                buffer_map[i].begin(), buffer_map[i].end());
        /*清空原本的信息*/
        buffer_map[i].clear();
    }

    /*日志信息排序*/
    sort(log_massage_list.begin(), log_massage_list.end(),
         [](LogMessage &x, LogMessage &y) -> bool {
             return x.record_time < y.record_time;
         });

    /*输出信息*/
    for (LogMessage &log_message: log_massage_list) {
        /*保存信息*/
        string result;
        /*生成日志信息*/
        log_message.ganerate_log_message(result);
        /*判断需不需要添加调用栈*/
        log_message.judge_traceback(result);
        /*进行输出*/
        log_message.out_message(result);
    }
    /*清空信息*/
    log_massage_list.clear();
}

/*设置buffer结束标志
 * return
 * */
void LogBuffer::set_stop_buffer_flag() {
    stop_buffer = true;
}
