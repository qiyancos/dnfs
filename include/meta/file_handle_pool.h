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
#ifndef DNFSD_FILE_HANDLE_POOL_H
#define DNFSD_FILE_HANDLE_POOL_H

#include <map>
#include "object_handle.h"
#include "base/persistent_base.h"
#include "utils/smart_ptr.h"
/*句柄管理池 , todo 单例*/
class FileHandlePool : public PersistentBase {
private:
    /*构建句柄池，id:句柄*/
    std::map<uint64_t, SmartPtr<ObjectHandle>> handle_pool;
public:
    /*添加
     * params object_handle:存储的句柄
     * */
    void push_fh(ObjectHandle &object_handle);
     /*todo 进行引用计数，引用消失删除*/
    /*得到
     * params fh_id:得到的句柄id
     * return 获取的句柄指针
     * */
    SmartPtr<ObjectHandle> get_fh(const uint64_t &fh_id);

    /*持久化
     * params path:持久化到的文件
     * */
    void persist(const std::string &persisence_path) override;

    /*读取持久化文件
    * params path:读取的持久化文件
    * */
    void resolve(const std::string &resolve_path) override;

    /*释放所有句柄*/
    ~FileHandlePool();
};


#endif //DNFSD_FILE_HANDLE_POOL_H
