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
#include <list>
#include "object_handle.h"
#include "base/persistent_base.h"
#include "utils/smart_ptr.h"
#include "object_info_base.h"

/*相同短hash查询链表*/
struct ObjectHandleList {
    std::list<SmartPtr> handle_list = {};
public:
    /*建立数据列表*/
    explicit ObjectHandleList(SmartPtrValue *tr, SmartPtrPool *pool);

    /*数据列表追加数据*/
    void push(SmartPtrValue *tr, SmartPtrPool *pool);

    /*遍历列表删除数据*/
    void delete_data(SmartPtrValue *tr);
};

/*句柄管理池 , todo 单例*/
class ObjectHandlePool : public PersistentBase, public SmartPtrPool {
private:
    /*构建句柄池，将nfs句柄转为系统句柄*/
    std::map<uint64_t, ObjectHandleList *> handle_pool;

public:
    /*默认构造函数*/
    ObjectHandlePool() = default;

    /*添加
     * params ObjectHandle:需要存储的句柄
     * */
    void
    push_fh(ObjectHandle &object_handle);

    /*得到
     * params fh3:查询的nfs3 handle
     * return 获取的句柄
     * */
    SmartPtr get_fh(const nfs_fh3 &fh3);

    /*刪除数据*/
    void delete_item(const uint64_t &delete_key, SmartPtrValue *smart_ptr_value) override;

    /*持久化
     * params path:持久化到的文件
     * */
    void persist(const std::string &persisence_path) override;

    /*读取持久化文件
    * params path:读取的持久化文件
    * */
    void resolve(const std::string &resolve_path) override;

    /*释放所有句柄*/
    ~ObjectHandlePool();
};


#endif //DNFSD_OBJECT_HANDLE_POOL_H
