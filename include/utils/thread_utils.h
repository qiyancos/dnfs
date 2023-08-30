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
    /* 线程ID到名字的映射 */
    static std::map<std::thread::id, std::string> thread_id_name_map;

    /* 线程名字到ID的映射 */
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

    /* 启动一个新的线程，并为线程起名，这个线程不存在于线程池中 */
    template<typename _Callable, typename... _Args>
    static std::thread* start_thread(const std::string& thread_name,
                                        _Callable&& func, _Args&&... args) {
        std::thread* new_thread = new std::thread(std::forward<_Callable>(func),
                               std::forward<_Args>(args)...);
        thread_id_name_map.emplace(new_thread->get_id(), thread_name);
        thread_name_id_map.emplace(thread_name, new_thread->get_id());
        return new_thread;
    }

private:
    /* 最大的线程数量限制 */
    int _max_thread_size;

    /* 所有线程池内线程用于执行关闭的锁信号 */
    std::mutex shutdown_lock;
    std::condition_variable shutdown_cond;

    /* 系统是否已经关闭的状态信息 */
    bool sys_shutdown = false;

    /* 当前线程池 */

public:
    ThreadPool(const int max_thread_size) {
        _max_thread_size = max_thread_size;
    }

    /* 重新设置线程池的线程大小个数 */
    void set_max_thread_size(const int max_thread_size);

    /* 等待系统关闭信号 */
    void wait_shutdown();

    /* 执行关闭操作所有的线程会在收到信号后关闭 */
    void shutdown();

    /* 等待所有线程退出 */
    void join();
};

#endif //UTILS_THREAD_UTILS_H
