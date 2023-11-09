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
#ifndef DNFSD_FSAL_HANDLE_H
#define DNFSD_FSAL_HANDLE_H

#include "map"
#include "string"
#include "mutex"
#include "nfs_args.h"

/*句柄操作类*/
class FsalHandle {
private:
    /*路径句柄map*/
    std::map<std::string, f_handle> handle_map = {};
    /*创建句柄锁*/
    std::mutex create_mtx;
    /*空句柄*/
    f_handle n_handle = {-1};
    /*默认锁属性*/
    static pthread_rwlockattr_t default_rwlock_attr;
private:
    /*默认构造函数*/
    FsalHandle();

public:
    /*获取句柄单例*/
    static FsalHandle &get_instance();

    /*判断句柄是否存在
     * params path:获取的路径
     * return 是否存在句柄
     * */
    bool judge_handle_exist(const std::string &path);

    /*添加文件句柄
     * params path:创建句柄的路径
     * return 是否创建成功
     * */
    bool push_handle(const std::string &path);

    /*获取文件句柄，没有创建
     * params path:获取句柄的路径
     * return 获取的句柄，，n_handle获取失败
     * */
    f_handle* get_handle(const std::string &path);

    /*仅获取文件句柄
     * params path:获取句柄的路径
     * return 获取的句柄，n_handle获取失败
     * */
    f_handle* just_get_handle(const std::string &path);

    /*创建句柄
     * params rwlock:创建的读写锁
     * params attr:创建的属性
     * */
    static void pthread_lock_init(pthread_rwlock_t *rwlock,pthread_rwlockattr_t *attr);

    /*删除句柄
     * params rwlock:删除的读写锁
     * */
    static void pthread_lock_destory(pthread_rwlock_t *rwlock);

    /*锁住句柄
     * params rwlock:需要锁的读写锁
     * */
    static void pthread_lock_write(pthread_rwlock_t *rwlock);

    /*锁住句柄
     * params rwlock:需要锁的读写锁
     * */
    static void pthread_lock_read(pthread_rwlock_t *rwlock);

    /*释放句柄
     * params rwlock:需要释放的读写锁
     * */
    static void pthread_unlock_rw(pthread_rwlock_t *rwlock);

    /*关闭所有的文件句柄*/
    void close_handles();
};

/*全局唯一日志实例*/
extern FsalHandle &fsal_handle;
#endif //DNFSD_FSAL_HANDLE_H
