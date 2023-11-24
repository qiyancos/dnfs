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
#include "base/persistent_base.h"

/*文件句柄管理*/
class ObjectHandle : public PersistentBase {
private:
    u_int data_len;
    char *data_val;
public:
    ObjectHandle() = default;
};


#endif //DNFSD_OBJECT_HANDLE_H
