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
#ifndef DNFSD_OBJECT_HANDLE_H
#define DNFSD_OBJECT_HANDLE_H

#include <string>
#include <atomic>
#include "base/persistent_base.h"
#include "file/file_handle.h"

/*文件句柄管理*/
class ObjectHandle : public PersistentBase {
private:
    /*二进制hash*/
    char hash[8];
    /*创建文件时间戳（纳秒）*/
    uint64_t build_time_nanoseconds;
public:
    /*构造函数
     * params fh:获取数据的文件句柄
     * */
    explicit ObjectHandle(struct nfs_fh3 &fh);

    /*生成文件句柄。/
     * params fh:生成的文件句柄
     * */
    void generate_nfs_fh(struct nfs_fh3 &fh) const;

    /*获取id*/
    uint64_t get_data_id() const;

    ~ObjectHandle() = default;
};

#endif // DNFSD_OBJECT_HANDLE_H
