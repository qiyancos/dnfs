/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Rock Lee lsk_mprc@pku.edu.cn
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License; This program is
 * distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the MIT license for
 * more details. You should have received a copy of the MIT License
 * along with this project.
 *
 */

#include <sstream>

#include "log/log.h"
#include "utils/thread_utils.h"

using namespace std;

#define MODULE_NAME "thread_pool"

/*id name存储字典*/
std::map<std::thread::id, std::string> ThreadPool:: thread_id_name_map;
/*name id存储字典*/
std::map<std::string, std::thread::id> ThreadPool:: thread_name_id_map;

// 设置当前现成的名称与线程id绑定
void ThreadPool::set_thread_name(const string &name) {
    const thread::id tid = this_thread::get_id();
    if (thread_name_id_map.find(name) != thread_name_id_map.end()) {
        // 如果发现线程名称出现重复，则会直接触发异常
        LOG(MODULE_NAME, EXIT_ERROR,
                   "Thread named \"%s\" already exists", name.c_str());
    }
    thread_name_id_map.emplace(name, tid);
    if (thread_id_name_map.find(tid) != thread_id_name_map.end()) {
        LOG(MODULE_NAME,D_INFO,
                   "Reset thread name from \"%s\" to \"%s\" for thread-%d",
                    thread_id_name_map[tid].c_str(), name.c_str(), tid);
    } else {
        LOG(MODULE_NAME,D_INFO,
                   "Set thread %ld name as \"%s\"", tid, name.c_str());
    }
    thread_id_name_map.emplace(tid, name);
}

// 设置当前现成的名称与线程id绑定
[[maybe_unused]] void ThreadPool::set_thread_name(const char* name) {
    const string name_str = name;
    set_thread_name(name_str);
}

// 获取当前线程的线程名称，如果没有设置返回id
[[maybe_unused]] string ThreadPool::get_thread_name(){
    const thread::id tid = this_thread::get_id();
    return get_target_thread_name(tid);
}

// 获取指定线程的线程名称，如果没有设置返回id
string ThreadPool::get_target_thread_name(const std::thread::id& tid){
    if (thread_id_name_map.find(tid) != thread_id_name_map.end()) {
        return thread_id_name_map[tid];
    } else {
        string tid_str = "Thread-";
        stringstream tid_ss;
        tid_ss<<tid_str << tid;
        return tid_ss.str();
    }
}

/* 重新设置线程池的线程大小个数 */
void ThreadPool::set_max_thread_size(const int max_thread_size) {
    _max_thread_size = max_thread_size;
}

/* 等待系统关闭信号 */
void ThreadPool::wait_shutdown() {
    unique_lock<mutex> shutdown_lck(shutdown_lock);
    shutdown_cond.wait(shutdown_lck);
}

/* 执行关闭操作所有的线程会在收到信号后关闭 */
void ThreadPool::shutdown() {
    unique_lock<mutex> shutdown_lck(shutdown_lock);
    shutdown_cond.notify_all();
}

/* 等待所有线程退出 */
void ThreadPool::join() {

}