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
#include "meta/object_handle.h"

/*构造函数
 * params fh:获取数据的文件句柄
 * */
ObjectHandle::ObjectHandle(const nfs_fh3 &fh) {

}

/*生成文件句柄
 * params fh:生成的文件句柄
 * */
void ObjectHandle::generate_nfs_fh(nfs_fh3 &fh) const {

}

/*获取建立时间*/
uint64_t ObjectHandle::get_build_time() const {
    return build_time_nanoseconds;
}

/*删除数据虚函数*/
uint64_t ObjectHandle::get_id() {
    return bin_hash;
}

/*相同类型数据对照方法*/
bool ObjectHandle::compore_data(SmartPtrValue *compare_value) {
    auto *obj = (ObjectHandle *) compare_value;
    return build_time_nanoseconds == obj->get_build_time();
}
