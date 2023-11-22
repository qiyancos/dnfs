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
#ifndef DNFSD_FILE_HANDLE_H
#define DNFSD_FILE_HANDLE_H

#include "string"
#include "atomic"

/*文件句柄管理*/
struct FileHandle {
    u_int data_len;
    char *data_val;
};


#endif //DNFSD_FILE_HANDLE_H
