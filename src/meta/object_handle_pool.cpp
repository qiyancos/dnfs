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
#include "meta/object_handle_pool.h"

/*建立数据列表*/
ObjectHandleList::ObjectHandleList(SmartPtrValue *tr, SmartPtrPool *pool) {
    handle_list.emplace_back(SmartPtr::Ptrs({tr, pool}));
}

/*数据列表追加数据*/
void ObjectHandleList::push(SmartPtrValue *tr, SmartPtrPool *pool) {
    handle_list.emplace_back(SmartPtr::Ptrs({tr, pool}));
}

/*遍历列表删除数据*/
void ObjectHandleList::delete_data(SmartPtrValue *tr) {
    for (auto &item: handle_list) {
        if (tr->compore_data(item.get_ptr())) {
            /*释放数据*/
            item.realse();
        }
    }
}

/*添加 todo 这里是不是应该传f3的handle过来
 * params ObjectHandle:需要存储的句柄
 * */
void ObjectHandlePool::push_fh(ObjectHandle &object_handle) {
    /*先为句柄分配空间*/
    auto *obj_handle_ptr = new ObjectHandle(object_handle);

    /*查询是否已经存在*/
    auto item = handle_pool.find(obj_handle_ptr->get_id());
    /*存在尽心追加*/
    if (item != handle_pool.end()) {
        item->second->push(obj_handle_ptr, this);
    } else {
        /*不存在创建*/
        auto *da = new ObjectHandleList(obj_handle_ptr, this);
        handle_pool.emplace(obj_handle_ptr->get_id(), da);
    }
}

/*得到
 * params fh3:查询的nfs3 handle
 * return 获取的句柄
 * */
SmartPtr ObjectHandlePool::get_fh(const nfs_fh3 &fh3) {
    /*查询是否已经存在*/
    /*todo 使用fh3偏移数据进行查询
     * 查到了进行比对返回
     * 没查到添加数据*/
}

void ObjectHandlePool::delete_item(const uint64_t &delete_key,
                                   SmartPtrValue *smart_ptr_value) {
    /*先获取需要删除的数据*/
    auto d_data = handle_pool.find(delete_key);
    if (d_data != handle_pool.end()) {
        /*先清空内存*/
        d_data->second->delete_data(smart_ptr_value);
    }
    /*在删除从队列删除*/
    handle_pool.erase(delete_key);
}

/*释放句柄池空间*/
ObjectHandlePool::~ObjectHandlePool() {
    /*遍历式释放空间*/
    for (auto handle_list: handle_pool) {
        delete handle_list.second;
    }
}
