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
ObjectHandle::ObjectHandle(struct nfs_fh3 &fh) {
    this->data_val = (uint64_t) fh.data.data_val;
    this->data_len = fh.data.data_len;
}

/*生成文件句柄
 * params fh:生成的文件句柄
 * */
void ObjectHandle::generate_nfs_fh(nfs_fh3 &fh) {
    fh.data.data_len = this->data_len;
    fh.data.data_val = (char *) this->data_val;
}
