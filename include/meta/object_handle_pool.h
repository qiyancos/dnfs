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
#ifndef DNFSD_OBJECT_HANDLE_POOL_H
#define DNFSD_OBJECT_HANDLE_POOL_H

#include <map>
#include "object_handle.h"
#include "base/persistent_base.h"
#include "utils/smart_ptr.h"
#include "object_info_base.h"

/*句柄管理池 , todo 单例*/
class ObjectHandlePool : public PersistentBase, SmartPtrPool {
private:
    /*构建句柄池，id:句柄*/
    std::map<SmartPtr<ObjectHandle> *, ObjectInfoBase *> handle_pool;

public:
    /*默认构造函数*/
    ObjectHandlePool() = default;

    /*添加
     * params smart_object_handle:智能指针数据
     * params object_base:
     * */
    void push_fh(SmartPtr<ObjectHandle> &smart_object_handle,ObjectInfoBase *object_base);

    /*得到
     * params fh_id:得到的句柄id
     * return 获取的句柄指针
     * */
    SmartPtr<ObjectHandle> get_fh(const uint64_t &fh_id);

    /*刪除数据*/
    void delete_item(void *key) override;

    /*持久化
     * params path:持久化到的文件
     * */
    void persist(const std::string &persisence_path) override;

    /*读取持久化文件
    * params path:读取的持久化文件
    * */
    void resolve(const std::string &resolve_path) override;

    /*释放所有句柄*/
    ~ObjectHandlePool() = default;
};


#endif //DNFSD_OBJECT_HANDLE_POOL_H
