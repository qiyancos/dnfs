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
#include "utils/smart_ptr.h"
#include "base/persistent_base.h"
#include "file/file_handle.h"

/*文件句柄管理*/
class ObjectHandle : public SmartPtrValue, public PersistentBase {
private:
    /*二进制hash*/
    uint64_t bin_hash;
    /*创建文件时间戳（纳秒）uint64_t*/
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

    /*获取建立时间*/
    [[nodiscard]] uint64_t get_build_time() const;

    /*删除数据虚函数*/
    uint64_t get_id() override;

    /*相同类型数据对照方法*/
    bool compore_data(SmartPtrValue *compare_value) override;

    /*持久化
     * params path:持久化到的文件
     * */
    void persist(const std::string &persisence_path) override;

    /*读取持久化文件
     * params path:读取的持久化文件
     * */
    void resolve(const std::string &resolve_path) override;

    ~ObjectHandle() = default;
};

#endif // DNFSD_OBJECT_HANDLE_H
