/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Rock Lee lsk_mprc@pku.edu.cn
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

#ifndef UTILS_THREAD_UTILS_H
#define UTILS_THREAD_UTILS_H

#include <thread>
#include <string>
#include <map>

class ThreadPool {
private:
    static std::map<std::thread::id, std::string> thread_id_name_map;
    static std::map<std::string, std::thread::id> thread_name_id_map;

public:
    // 设置当前现成的名称与线程id绑定
    static void set_thread_name(const std::string& name);
    // 设置当前现成的名称与线程id绑定
    [[maybe_unused]] static void set_thread_name(const char* name);
    // 获取当前线程的线程名称，如果没有设置返回id
    [[maybe_unused]] static std::string get_thread_name();
    // 获取指定线程的线程名称，如果没有设置返回id
    static std::string get_target_thread_name(const std::thread::id& tid);
};

#endif //UTILS_THREAD_UTILS_H
