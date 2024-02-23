/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Jiao Yue 3059497228@qq.com
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
#include "file/fsal_handle.h"
#include "log/log.h"
#include "utils/common_utils.h"

#define MODULE_NAME "NFS"

using namespace std;

/*默认构造函数*/
FsalHandle::FsalHandle() = default;

/*默认锁属性*/
pthread_rwlockattr_t FsalHandle::default_rwlock_attr;
pthread_mutexattr_t FsalHandle::default_mutex_attr;

/*建立句柄实例*/
FsalHandle &fsal_handle = FsalHandle::get_instance();

/*返回创建的句柄操作单例*/
FsalHandle &FsalHandle::get_instance() {
    /*建立单例*/
    static FsalHandle fsal_handle;
    return fsal_handle;
}

/*判断句柄是否存在
 * params path:获取的路径
 * return 是否存在句柄
 * */
bool FsalHandle::judge_handle_exist(const std::string &path) {
    /*不存在返回false*/
    if (handle_map.find(path) == handle_map.end()) {
        return false;
    }
    return true;
}

/*添加文件句柄
 * params path:创建句柄的路径
 * return 是否创建成功
 * */
bool FsalHandle::push_handle(const string &path) {
    /*创建的句柄*/
    int file_handle;
    /*如果没有建立文件句柄*/
    /*自动上锁*/
    unique_lock<mutex> create_uk(create_mtx);
    if (!judge_handle_exist(path)) {
        /*获取文件句柄*/
        file_handle = open(path.c_str(), O_CREAT | O_WRONLY);
        /*打开失败*/
        if (file_handle == -1) {
            LOG(MODULE_NAME, D_ERROR, "open file '%s' failed",
                path.c_str());
            return false;
        }
        /*创建句柄*/
        auto *handle = (f_handle *) gsh_calloc(1, sizeof(f_handle));
        /*初始化数据*/
        handle->want_write = 0;
        handle->want_read = 0;
        handle->fd_work = 0;
        handle->io_work = 0;
        /*保存句柄*/
        handle->handle = file_handle;
        /*初始化读写锁*/
        pthread_lock_init(&handle->handle_rwlock_lock, nullptr);
        /*初始化互斥锁*/
        pthread_w_mutex_init(&handle->work_mutex, nullptr);
        /*初始化通知信号*/
        pthread_w_cond_init(&handle->work_cond, nullptr);
        /*初始化share的*/
        handle->share = (fsal_share *) gsh_calloc(1, sizeof(fsal_share));
        /*添加句柄*/
        handle_map[path] = handle;
    }
    return true;
}

/*获取文件句柄,没有创建
 * params path:获取句柄的路径
 * return 获取的句柄，n_handle获取失败
 * */
f_handle *FsalHandle::get_handle(const string &path) {
    /*有则返回句柄*/
    if (judge_handle_exist(path)) {
        if (handle_map[path]->handle == -1)
            /*打开句柄*/
            handle_map[path]->handle = open(path.c_str(), O_CREAT | O_WRONLY);
        return handle_map[path];
    } else {
        /*不存在创建句柄返回*/
        if (push_handle(path)) {
            return handle_map[path];
        }
    }
    /*都失败返回-1*/
    return &n_handle;
}

/*仅获取文件句柄
 * params path:获取句柄的路径
 * return 获取的句柄，n_handle获取失败
 * */
f_handle *FsalHandle::just_get_handle(const string &path) {
    /*有则返回句柄*/
    if (judge_handle_exist(path)) {
        return handle_map[path];
    }
    /*返回-1*/
    return &n_handle;
}

/*关闭句柄
 * params file_handle:需要关闭的句柄
 * */
void FsalHandle::close_handle(f_handle *file_handle) {
    if (file_handle->handle != -1) {
        close(file_handle->handle);
        file_handle->handle = -1;
    }
}


/*创建句柄
 * params rwlock:创建的读写锁
 * params attr:创建的属性
 * */
void FsalHandle::pthread_lock_init(pthread_rwlock_t *rwlock, pthread_rwlockattr_t *attr) {
    int rc;
    pthread_rwlockattr_t *_attr = attr;

    if (_attr == nullptr)
        _attr = &default_rwlock_attr;

    rc = pthread_rwlock_init(rwlock, _attr);
    if (rc == 0) {
        LOG(MODULE_NAME, D_INFO,
            "Init rwlock %p",
            rwlock);
    } else {
        LOG(MODULE_NAME, D_ERROR,
            "Error %d, Init rwlock %p", rc, rwlock);
        abort();
    }

}

/*删除句柄
 * params rwlock:删除的读写锁
 * */
void FsalHandle::pthread_lock_destory(pthread_rwlock_t *rwlock) {
    int rc;

    rc = pthread_rwlock_destroy(rwlock);
    if (rc == 0) {
        LOG(MODULE_NAME, D_INFO,
            "Destroy lock %p",
            rwlock);
    } else {
        LOG(MODULE_NAME, D_ERROR,
            "Error %d, Destroy lock %p", rc, rwlock);
        abort();
    }
}

/*锁住句柄
 * params rwlock:需要锁的读写锁
 * */
void FsalHandle::pthread_lock_write(pthread_rwlock_t *rwlock) {
    int rc;
    rc = pthread_rwlock_wrlock(rwlock);
    if (rc == 0) {
        LOG(MODULE_NAME, D_INFO,
            "Get write lock on %p",
            rwlock);
    } else {
        LOG(MODULE_NAME, D_ERROR,
            "Error %d, write locking %p",
            rc, rwlock);
        abort();
    }
}

/*释放句柄
 * params rwlock:需要释放的读写锁
 * */
void FsalHandle::pthread_lock_read(pthread_rwlock_t *rwlock) {
    int rc;
    rc = pthread_rwlock_rdlock(rwlock);
    if (rc == 0) {
        LOG(MODULE_NAME, D_INFO,
            "Get read lock on %p",
            rwlock);
    } else {
        LOG(MODULE_NAME, D_INFO,
            "Error %d, read locking %p",
            rc, rwlock);
        abort();
    }
}

/*释放句柄
 * params rwlock:需要释放的读写锁
 * */
void FsalHandle::pthread_unlock_rw(pthread_rwlock_t *rwlock) {
    int rc;
    rc = pthread_rwlock_unlock(rwlock);
    if (rc == 0) {
        LOG(MODULE_NAME, D_INFO,
            "Unlocked %p",
            rwlock);
    } else {
        LOG(MODULE_NAME, D_ERROR,
            "Error %d, unlocking %p",
            rc, rwlock);
        abort();
    }
}

/*初始化互斥锁
 * params work_mutex:初始化的互斥锁
 * params mutex_attr:初始化的属性
 * */
void FsalHandle::pthread_w_mutex_init(pthread_mutex_t *work_mutex,
                                      pthread_mutexattr_t *mutex_attr) {
    int rc;
    pthread_mutexattr_t *attr = mutex_attr;

    if (attr == nullptr)
        attr = &default_mutex_attr;

    rc = pthread_mutex_init(work_mutex, attr);

    if (rc == 0) {
        LOG(MODULE_NAME, D_INFO,
            "Init rwlock %p",
            work_mutex);
    } else {
        LOG(MODULE_NAME, D_ERROR,
            "Error %d, Init mutex %p", rc, work_mutex);
        abort();
    }
}

/*锁互斥锁
 * params work_mutex:初始化的互斥锁
 * */
void FsalHandle::pthread_w_mutex_lock(pthread_mutex_t *work_mutex) {
    int rc;

    rc = pthread_mutex_lock(work_mutex);
    if (rc == 0) {
        LOG(MODULE_NAME, D_INFO, "Acquired mutex %p", work_mutex);
    } else {
        LOG(MODULE_NAME, D_ERROR,
            "Error %d, acquiring mutex %p", rc, work_mutex);
        abort();
    }
}

/*解锁互斥锁
 * params work_mutex:初始化的互斥锁
 * */
void FsalHandle::pthread_w_mutex_unlock(pthread_mutex_t *work_mutex) {
    int rc;

    rc = pthread_mutex_unlock(work_mutex);
    if (rc == 0) {
        LOG(MODULE_NAME, D_INFO, "Released mutex %p", work_mutex);
    } else {
        LOG(MODULE_NAME, D_ERROR,
            "Error %d, releasing mutex %p", rc, work_mutex);
        abort();
    }
}

/*销毁互斥锁
 * params work_mutex:需要销毁的互斥锁
 * */
void FsalHandle::pthread_w_mutex_destroy(pthread_mutex_t *work_mutex) {
    int rc;

    rc = pthread_mutex_destroy(work_mutex);
    if (rc == 0) {
        LOG(MODULE_NAME, D_INFO,
            "Destroy mutex %p",
            work_mutex);
    } else {
        LOG(MODULE_NAME, D_ERROR,
            "Error %d, Destroy mutex %p", rc, work_mutex);
        abort();
    }
}

/*初始化通知
 * params cond:待初始化的通知信号
 * params cond_attr:信号属性
 * */
void FsalHandle::pthread_w_cond_init(pthread_cond_t *cond,
                                     pthread_condattr_t *cond_attr) {
    int rc;

    rc = pthread_cond_init(cond, cond_attr);
    if (rc == 0) {
        LOG(MODULE_NAME, D_INFO,
            "Init cond %p", cond);
    } else {
        LOG(MODULE_NAME, D_ERROR,
            "Error %d, Init cond %p (%s) ", rc, cond);
        abort();
    }
}

/*销毁通知信号
 * params cond:待销毁的信号
 * */
void FsalHandle::pthread_w_cond_destroy(pthread_cond_t *cond) {
    int rc;

    rc = pthread_cond_destroy(cond);
    if (rc == 0) {
        LOG(MODULE_NAME, D_INFO,
            "Destroy cond %p", cond);
    } else {
        LOG(MODULE_NAME, D_ERROR,
            "Error %d, Destroy cond %p", rc, cond);
        abort();
    }
}

/*等该信号通知
 * params cond:等待通知信号
 * params work_mutex:等待通知互斥锁
 * */
void FsalHandle::pthread_w_cond_wait(pthread_cond_t *cond,
                                     pthread_mutex_t *work_mutex) {
    int rc;

    rc = pthread_cond_wait(cond, work_mutex);
    if (rc == 0) {
        LOG(MODULE_NAME, D_INFO, "Wait cond %p mutex %p", cond, work_mutex);
    } else {
        LOG(MODULE_NAME, D_ERROR,
            "Error %d, Wait cond %p mutex", rc, cond, work_mutex);
        abort();
    }
}

/* 通知信号
 * params cond:等待通知信号
 * */
void FsalHandle::pthread_w_cond_signal(pthread_cond_t *cond) {
    int rc;

    rc = pthread_cond_signal(cond);
    if (rc == 0) {
        LOG(MODULE_NAME, D_INFO, "Signal cond %p", cond);
    } else {
        LOG(MODULE_NAME, D_ERROR,
            "Error %d, Signal cond %p ", rc, cond);
        abort();
    }
}

/*关闭所有的文件句柄*/
void FsalHandle::close_handles() {
    /*遍历关闭句柄*/
    for (auto &handle: handle_map) {
        /*关闭句柄*/
        close(handle.second->handle);
        /*释放share空间*/
        gsh_free(handle.second->share);
        /*销毁读写锁*/
        pthread_lock_destory(&handle.second->handle_rwlock_lock);
        /*销毁互斥锁*/
        pthread_w_mutex_destroy(&handle.second->work_mutex);
        /*销毁通知信号*/
        pthread_w_cond_destroy(&handle.second->work_cond);
        /*释放整个空间*/
        gsh_free(handle.second);
    }
    /*清空句柄map*/
    handle_map.clear();
}

