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
#include "nfs/nfs_args.h"

/*句柄结构体*/
struct f_handle {
    /*获取的句柄*/
    int handle;
    /*线程读写锁*/
    pthread_rwlock_t handle_rwlock_lock;
    pthread_mutex_t work_mutex;
    pthread_cond_t work_cond;
    /*判断现在是否在工作*/
    int32_t io_work;
    int32_t want_read;
    int32_t want_write;
    int32_t fd_work;
    struct fsal_share *share;
};

struct fsal_share {
    unsigned int share_access_read;
    unsigned int share_access_write;
    unsigned int share_deny_read;
    unsigned int share_deny_write;
    /**< Count of mandatory share deny write */
    unsigned int share_deny_write_mand;
};

/*句柄操作类*/
class FsalHandle {
private:
    /*路径句柄map*/
    std::map<std::string, f_handle *> handle_map = {};
    /*创建句柄锁*/
    std::mutex create_mtx;
    /*空句柄*/
    f_handle n_handle = {-1};
    /*默认锁属性*/
    static pthread_rwlockattr_t default_rwlock_attr;
    static pthread_mutexattr_t default_mutex_attr;
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
    f_handle *get_handle(const std::string &path);

    /*仅获取文件句柄
     * params path:获取句柄的路径
     * return 获取的句柄，n_handle获取失败
     * */
    f_handle *just_get_handle(const std::string &path);

    /*关闭句柄
     * params file_handle:需要关闭的句柄
     * */
    static void close_handle(f_handle *file_handle);

    /*创建句柄
     * params rwlock:创建的读写锁
     * params attr:创建的属性
     * */
    static void pthread_lock_init(pthread_rwlock_t *rwlock, pthread_rwlockattr_t *attr);

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

    /*初始化互斥锁
     * params work_mutex:初始化的互斥锁
     * params mutex_attr:初始化的属性
     * */
    static void
    pthread_w_mutex_init(pthread_mutex_t *work_mutex, pthread_mutexattr_t *mutex_attr);

    /*锁互斥锁
     * params work_mutex:初始化的互斥锁
     * */
    static void pthread_w_mutex_lock(pthread_mutex_t *work_mutex);

    /*解锁互斥锁
     * params work_mutex:初始化的互斥锁
     * */
    static void pthread_w_mutex_unlock(pthread_mutex_t *work_mutex);

    /*销毁互斥锁
     * params work_mutex:需要销毁的互斥锁
     * */
    static void pthread_w_mutex_destroy(pthread_mutex_t *work_mutex);

    /*初始化通知
     * params cond:待初始化的通知信号
     * params cond_attr:信号属性
     * */
    static void pthread_w_cond_init(pthread_cond_t *cond, pthread_condattr_t *cond_attr);

    /*销毁通知信号
     * params cond:待销毁的信号
     * */
    static void pthread_w_cond_destroy(pthread_cond_t *cond);

    /*等该信号通知
     * params cond:等待通知信号
     * params work_mutex:等待通知互斥锁
     * */
    static void pthread_w_cond_wait(pthread_cond_t *cond, pthread_mutex_t *work_mutex);

    /* 通知信号
     * params cond:等待通知信号
     * */
    static void pthread_w_cond_signal(pthread_cond_t *cond);

    /*关闭所有的文件句柄*/
    void close_handles();
};

/*全局唯一日志实例*/
extern FsalHandle &fsal_handle;
#endif //DNFSD_FSAL_HANDLE_H
