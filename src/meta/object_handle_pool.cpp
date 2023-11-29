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

/*添加
 * params smart_object_handle:智能指针数据
 * params object_base:文件信息
 * */
//void ObjectHandlePool::push_fh(SmartPtr<ObjectHandle> &smart_object_handle,
//                               ObjectInfoBase *object_base) {
//    /*先看有没有数据*/
//    auto data = handle_pool.find(&smart_object_handle);
//    /*如果存在更新数据*/
//    if (data != handle_pool.end()) {
//        /*存储数据*/
//        data->second = object_base;
//    } else {
//        /*直接插入数据*/
//        handle_pool.emplace(&smart_object_handle, &this, object_base);
//    }
//}
//
///*得到
// * params fh_id:得到的句柄id
// * return 获取的句柄指针
// * */
//SmartPtr<ObjectHandle> ObjectHandlePool::get_fh(const uint64_t &fh_id) {
//    /*先查找看看存不存在*/
//    auto data = handle_pool.find(fh_id);
//    /*如果存在更新数据*/
//    if (data != handle_pool.end()) {
//        return data->second;
//    } else {
//        /*todo 去文件找*/
//        return SmartPtr<ObjectHandle>(nullptr, nullptr);
//    }
//}

void ObjectHandlePool::delete_item(void *key) {
    handle_pool.erase((SmartPtr<ObjectHandle>*)key);
}
