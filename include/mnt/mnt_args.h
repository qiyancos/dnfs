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
#ifndef DNFSD_MNT_ARGS_H
#define DNFSD_MNT_ARGS_H
extern "C" {
#include "rpc/svc.h"
}

#include "file/file_handle.h"

#define XDR_ARRAY_MAXLEN 1024

#define    MNTPATHLEN 1024

#define    MNTNAMLEN 255

typedef char *mnt3_name;

typedef char *mnt3_dirpath;

/*存放mnt基础数据*/
enum mountstat3 {
    MNT3_OK = 0,
    MNT3ERR_PERM = 1,
    MNT3ERR_NOENT = 2,
    MNT3ERR_IO = 5,
    MNT3ERR_ACCES = 13,
    MNT3ERR_NOTDIR = 20,
    MNT3ERR_INVAL = 22,
    MNT3ERR_NAMETOOLONG = 63,
    MNT3ERR_NOTSUPP = 10004,
    MNT3ERR_SERVERFAULT = 10006
};

/*声明数据参数*/
union nfs_arg_t;
union nfs_res_t;
#endif //DNFSD_MNT_ARGS_H
