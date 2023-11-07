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

/*句柄操作类*/
class FsalHandle {
private:
    /*路径句柄map*/
    std::map<std::string, int> handle_map = {};
    /*创建句柄锁*/
    std::mutex create_mtx;
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
     * return 获取的句柄，-1获取失败
     * */
    int get_handle(const std::string &path);

    /*仅获取文件句柄
     * params path:获取句柄的路径
     * return 获取的句柄，-1获取失败
     * */
    int just_get_handle(const std::string &path);

    /*关闭所有的文件句柄*/
    void close_handles();
};

/*全局唯一日志实例*/
extern FsalHandle &fsal_handle;
#endif //DNFSD_FSAL_HANDLE_H