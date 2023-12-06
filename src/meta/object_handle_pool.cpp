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
#include "meta/object_handle_pool.h"

/*建立数据列表
 * params ptr:存储数据指针
 * params pool:数据池指针
 * */
ObjectHandleList::ObjectHandleList(SmartPtrValue *ptr, SmartPtrPool *pool) {
    handle_list.emplace_back(SmartPtr::Ptrs({ptr, pool}));
}

/*数据列表追加数据
 * params ptr:存储数据指针
 * params pool:数据池指针
 * */
void ObjectHandleList::push(SmartPtrValue *ptr, SmartPtrPool *pool) {
    handle_list.emplace_back(SmartPtr::Ptrs({ptr, pool}));
}

/*遍历列表删除数据
 * params delete_ptr:待删除的数据
 * */
void ObjectHandleList::delete_data(SmartPtrValue *delete_ptr) {
    for (auto &item: handle_list) {
        if (delete_ptr->compore_data(item.get_ptr())) {
            /*释放数据*/
            item.realse();
            break;
        }
    }
}

/*遍历查找数据列表
 * params serch_ptr:查找的数据
 * */
SmartPtr *ObjectHandleList::search_data(SmartPtrValue *serch_ptr) {
    for (auto &item: handle_list) {
        if (serch_ptr->compore_data(item.get_ptr())) {
            /*保存数据*/
            return &item;
        }
    }
    return nullptr;
}

/*添加
 * params smart_value:需要插入的数据，todo 思考需不需要加锁
 * return 创建的句柄智能指针
 * */
SmartPtr ObjectHandlePool::push_fh(SmartPtrValue *obj_handle_ptr) {
    /*查询是否已经存在*/
    auto item = handle_pool.find(obj_handle_ptr->get_id());
    /*存在尽心追加*/
    if (item != handle_pool.end()) {
        /*将句柄插进列表*/
        item->second->push(obj_handle_ptr, this);
        /*返回最后一个数据 todo 思考需不需要加锁*/
        return *item->second->handle_list.rbegin();
    } else {
        /*不存在创建*/
        auto *da = new ObjectHandleList(obj_handle_ptr, this);
        /*插入句柄管理池*/
        handle_pool.emplace(obj_handle_ptr->get_id(), da);
        /*直接返回第一个数据引用，todo 思考需不需要加锁*/
        return *da->handle_list.begin();
    }
}

/*得到
 * params fh3:查询的nfs3 handle
 * return 查询到的句柄智能指针
 * */
SmartPtr ObjectHandlePool::get_fh(const nfs_fh3 &fh3) {
    /*直接构造句柄*/
    auto *obj_handle = new ObjectHandle(fh3);
    /*查询数据是否存在*/
    auto item = handle_pool.find(obj_handle->get_id());
    /*保存查找到的数据*/
    SmartPtr *find_ptr;
    /*todo 在创建完数据后进行关联数据更新，比如更新binlog对应的文件信息*/
    if (item != handle_pool.end()) {
        /*遍历查找数据*/
        find_ptr = item->second->search_data(obj_handle);
        /*如果找到数据直接返回*/
        if (find_ptr) {
            /*释放临时的数据*/
            delete obj_handle;
            return *find_ptr;
        }
    }
    /*插入之后在返回*/
    return push_fh(obj_handle);
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
