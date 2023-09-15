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
#ifndef DNFSD_NFS_ARGS_H
#define DNFSD_NFS_ARGS_H

#include "file/file_handle.h"

typedef uint32_t nfs3_uint32;

typedef uint64_t nfs3_uint64;

typedef nfs3_uint32 uid3;

typedef nfs3_uint32 gid3;

typedef nfs3_uint32 mode3;

typedef nfs3_uint64 size3;

typedef nfs3_uint64 fileid3;
/* We use the fsal_types.h struct fsal_attrlist to avoid copying */
typedef struct fsal_attrlist fattr3;

typedef struct post_op_attr {
    bool_t attributes_follow;
    union {
        fattr3 attributes;
    } post_op_attr_u;
} post_op_attr;

typedef struct nfstime3 {
    nfs3_uint32 tv_sec;
    nfs3_uint32 tv_nsec;
} nfstime3;

typedef struct FSINFO3resok {
    post_op_attr obj_attributes;
    nfs3_uint32 rtmax;
    nfs3_uint32 rtpref;
    nfs3_uint32 rtmult;
    nfs3_uint32 wtmax;
    nfs3_uint32 wtpref;
    nfs3_uint32 wtmult;
    nfs3_uint32 dtpref;
    size3 maxfilesize;
    nfstime3 time_delta;
    nfs3_uint32 properties;
} FSINFO3resok;

typedef enum nfsstat3 {
    NFS3_OK = 0,
    NFS3ERR_PERM = 1,
    NFS3ERR_NOENT = 2,
    NFS3ERR_IO = 5,
    NFS3ERR_NXIO = 6,
    NFS3ERR_ACCES = 13,
    NFS3ERR_EXIST = 17,
    NFS3ERR_XDEV = 18,
    NFS3ERR_NODEV = 19,
    NFS3ERR_NOTDIR = 20,
    NFS3ERR_ISDIR = 21,
    NFS3ERR_INVAL = 22,
    NFS3ERR_FBIG = 27,
    NFS3ERR_NOSPC = 28,
    NFS3ERR_ROFS = 30,
    NFS3ERR_MLINK = 31,
    NFS3ERR_NAMETOOLONG = 63,
    NFS3ERR_NOTEMPTY = 66,
    NFS3ERR_DQUOT = 69,
    NFS3ERR_STALE = 70,
    NFS3ERR_REMOTE = 71,
    NFS3ERR_BADHANDLE = 10001,
    NFS3ERR_NOT_SYNC = 10002,
    NFS3ERR_BAD_COOKIE = 10003,
    NFS3ERR_NOTSUPP = 10004,
    NFS3ERR_TOOSMALL = 10005,
    NFS3ERR_SERVERFAULT = 10006,
    NFS3ERR_BADTYPE = 10007,
    NFS3ERR_JUKEBOX = 10008
} nfsstat3;

typedef struct FSINFO3resfail {
    post_op_attr obj_attributes;
} FSINFO3resfail;

typedef struct FSINFO3res {
    nfsstat3 status;
    union {
        FSINFO3resok resok;
        FSINFO3resfail resfail;
    } FSINFO3res_u;
} FSINFO3res;

struct FSINFO3args {
    nfs_fh3 fsroot;
};
typedef struct FSINFO3args FSINFO3args;

typedef union nfs_arg_ {
    FSINFO3args arg_fsinfo3;
} nfs_arg_t;

typedef union nfs_res_ {
    FSINFO3res res_fsinfo3;
} nfs_res_t;

typedef enum ftype3 {
    NF3REG = 1,
    NF3DIR = 2,
    NF3BLK = 3,
    NF3CHR = 4,
    NF3LNK = 5,
    NF3SOCK = 6,
    NF3FIFO = 7
} ftype3;

typedef struct specdata3 {
    nfs3_uint32 specdata1;
    nfs3_uint32 specdata2;
} specdata3;

#endif //DNFSD_NFS_ARGS_H
