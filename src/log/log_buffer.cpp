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

atomic<int> LogBuffer::log_num = 0;

LogBuffer::LogBuffer() = default;

/*将缓存写入文件,监听log_num*/
void LogBuffer::output_thread() {
    /*自动上锁*/
    unique_lock<mutex> write_uk(write_mtx, defer_lock);
    while (true) {
        /*加上锁，给wite使用*/
        write_uk.lock();
        /*等待锁*/
        cond.wait(write_uk);

        /*清空计数器*/
        log_num.store(0);
        /*日志信息存储列表*/
        vector<LogMessage> log_massage_list;

        /*遍历添加日志信息*/
        for (auto buffer: buffer_map) {
            /*追加信息*/
            log_massage_list.insert(log_massage_list.end(),
                                    buffer.second.begin(), buffer.second.end());
        }

        /*清空原本的信息*/
        buffer_map.clear();
        /*复制完数据解锁*/
        write_uk.unlock();

        /*日志信息排序*/
        sort(log_massage_list.begin(), log_massage_list.end(),
             [](LogMessage &x, LogMessage &y) -> bool {
                 return x.get_record_time() < y.get_record_time();
             });

        /*直接遍历输出*/
        for (LogMessage &log_message: log_massage_list) {
            /*保存信息*/
            string result;
            /*生成日志信息*/
            log_message.ganerate_log_message(result);
            /*判断需不需要添加调用栈*/
            log_message.judge_traceback(result, nullptr);
            /*进行输出*/
            log_message.out_message(result, nullptr);
        }
    }
}


/*设置缓存限制
 * params b_limit:设置的缓存限制
 * return
 * */
void LogBuffer::set_limit(const int &b_limit) {
    buffer_limit = b_limit;
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
        /*所有的操作都要在锁内进行*/
        /*自动上锁*/
        unique_lock<mutex> uk(mtx[hash_id]);

        /*查询是否已经存在thread_name对应的数据*/
        if (buffer_map.find(hash_id) == buffer_map.end()) {
            /*不存在,建立数据字典*/
            buffer_map[hash_id] = {};
        }
        /*已经存在进行添加*/
        buffer_map[hash_id].push_back(log_message);
    }

    /*加入*/
    /*数据总数加1*/
    log_num += 1;
    unique_lock<mutex> write_uk(write_mtx);
    /*如果数目大于设置的缓存限制*/
    if (log_num > buffer_limit) {
        cond.notify_one();
    }
}
