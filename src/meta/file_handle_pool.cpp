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
#include "meta/file_handle_pool.h"

/*添加
 * params object_handle:存储的句柄
 * */
void FileHandlePool::push_fh(ObjectHandle &object_handle) {
    /*先看有没有数据*/
    auto data = handle_pool.find(object_handle.get_data_id());
    /*如果存在更新数据*/
    if (data != handle_pool.end()) {
        /*构造数据智能指针*/
        SmartPtr<ObjectHandle> obj_smart(&object_handle);
        /*存储数据*/
        data->second = obj_smart;
    } else {
        /*直接插入数据*/
        handle_pool.emplace(object_handle.get_data_id(), &object_handle);
    }
}
