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
#include "nfs/fsal_handle.h"
#include "log/log.h"

#define MODULE_NAME "NFS"

using namespace std;

/*默认构造函数*/
FsalHandle::FsalHandle() = default;

/*默认锁属性*/
pthread_rwlockattr_t FsalHandle::default_rwlock_attr;

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
        f_handle handle{file_handle};
        /*初始化锁*/
        pthread_lock_init(&handle.handle_rwlock_lock, nullptr);
        /*添加句柄*/
        handle_map[path] = handle;
    }
    return true;
}

/*获取文件句柄,没有创建
 * params path:获取句柄的路径
 * return 获取的句柄，n_handle获取失败
 * */
f_handle FsalHandle::get_handle(const string &path) {
    /*有则返回句柄*/
    if (judge_handle_exist(path)) {
        return handle_map[path];
    } else {
        /*不存在创建句柄返回*/
        if (push_handle(path)) {
            return handle_map[path];
        }
    }
    /*都失败返回-1*/
    return n_handle;
}

/*仅获取文件句柄
 * params path:获取句柄的路径
 * return 获取的句柄，n_handle获取失败
 * */
f_handle FsalHandle::just_get_handle(const string &path) {
    /*有则返回句柄*/
    if (judge_handle_exist(path)) {
        return handle_map[path];
    }
    /*返回-1*/
    return n_handle;
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
        LOG(MODULE_NAME, L_WARN,
            "Init rwlock %p",
            rwlock);
    } else {
        LOG(MODULE_NAME, L_WARN,
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
        LOG(MODULE_NAME, L_WARN,
            "Destroy mutex %p",
            rwlock);
    } else {
        LOG(MODULE_NAME, L_WARN,
            "Error %d, Destroy mutex %p", rc, rwlock);
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
        LOG(MODULE_NAME, L_WARN,
            "Got write lock on %p",
            rwlock);
    } else {
        LOG(MODULE_NAME, L_WARN,
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
            "Got read lock on %p",
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
        LOG(MODULE_NAME, L_WARN,
            "Unlocked %p",
            rwlock);
    } else {
        LOG(MODULE_NAME, L_WARN,
            "Error %d, unlocking %p",
            rc, rwlock);
        abort();
    }
}


/*关闭所有的文件句柄*/
void FsalHandle::close_handles() {
    /*遍历关闭句柄*/
    for (auto &handle: handle_map) {
        /*关闭句柄*/
        close(handle.second.handle);
        /*销毁锁*/
        pthread_lock_destory(&(handle.second.handle_rwlock_lock));
    }
    /*清空句柄map*/
    handle_map.clear();
}


