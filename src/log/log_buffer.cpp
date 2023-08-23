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

/*建立日志缓存实例*/
LogBuffer log_buffer=LogBuffer::get_instance();

LogBuffer::LogBuffer() = default;

/*将缓存写入文件,监听log_num*/
void LogBuffer::output_thread() {
    /*todo 开启监听线程*/
    cout<<"缓存线程开启"<<endl;
}

/*得到日志缓存单例对象
 * return: 日志缓存对象
 * */
LogBuffer &LogBuffer::get_instance() {
    /*建立单例*/
    static LogBuffer log_buffer;
    return log_buffer;
}

/*设置缓存限制
 * params b_limit:设置的缓存限制
 * return
 * */
void LogBuffer::set_limit(const int &b_limit) {
    buffer_limit = b_limit;
}

/*添加线程对应的日志，每次记录log_num递增
 * params thread_name:线程名
 * params log_message:打印信息保存对象
 * return
 * */
void LogBuffer::add_log_buffer(const string &thread_name,
                               const LogMessage &log_message) {
    /*todo 枷锁进行添加缓存*/

}
