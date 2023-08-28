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

/*设置线程锁*/
mutex mtx;
/*设置条件锁通知缓存写文件线程*/
condition_variable cond;

atomic<int> LogBuffer::log_num = 0;

LogBuffer::LogBuffer() = default;

/*将缓存写入文件,监听log_num*/
void LogBuffer::output_thread() {
    while (true) {
        /*自动上锁*/
        std::unique_lock<std::mutex> uk(mtx);
        /*等待锁*/
        cond.wait(uk);
        /*清空计数器*/
        log_num.store(0);
        /*复制日志缓存数据*/
        std::map<int, std::vector<LogMessage>> save_buffer;

        copy(buffer_map.begin(),buffer_map.end(),inserter(save_buffer,save_buffer.begin()));
        /*清空原本的信息*/
        buffer_map.clear();

        /*这里先直接遍历打印*/
        for (auto &buffer: save_buffer) {
            for (LogMessage &log_message: buffer.second) {
                string result;
                string *error_info;
                string s;
                error_info = &s;
                if (log_message.grnarate_log_message(result, error_info) == 0) {
                    cout << result << endl;
                } else {
                    cout << *error_info << endl;
                }
            }
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
void LogBuffer::add_log_buffer(const int &thread_id,
                               const LogMessage &log_message) {
    /*所有的操作都要在锁内进行*/
    /*自动上锁*/
    std::unique_lock<std::mutex> uk(mtx);

    /*进行哈希计算*/
    int hash_id=thread_id%10;

    /*查询是否已经存在thread_name对应的数据*/
    if (buffer_map.find(hash_id) == buffer_map.end()) {
        /*不存在,建立数据字典*/
        buffer_map[hash_id] = {};
    }
    /*已经存在进行添加*/
    buffer_map[hash_id].push_back(log_message);

    /*数据总数加1*/
    log_num += 1;
    cout<<log_num<<endl;

    /*如果数目大于设置的缓存限制*/
    if (log_num > buffer_limit) {
        cond.notify_one();
    }
}
