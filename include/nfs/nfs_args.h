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
/*存放nfs基础数据*/
#ifndef DNFSD_NFS_ARGS_H
#define DNFSD_NFS_ARGS_H
extern "C" {
#include "rpc/svc.h"
}
#include <sys/stat.h>

#include "file/file_handle.h"

#define NFS3_FHSIZE 64

typedef int32_t bool_t;

typedef uint32_t nfs3_uint32;

typedef uint64_t nfs3_uint64;

typedef nfs3_uint32 uid3;

typedef nfs3_uint32 gid3;

typedef nfs3_uint32 mode3;

typedef nfs3_uint64 size3;

typedef nfs3_uint64 fileid3;

typedef struct nfstime3 {
    nfs3_uint32 tv_sec;
    nfs3_uint32 tv_nsec;
} nfstime3;

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

struct fattr3
{
    ftype3 type;
    mode3 mode;
    nfs3_uint32 nlink;
    uid3 uid;
    gid3 gid;
    size3 size;
    size3 used;
    specdata3 rdev;
    nfs3_uint64 fsid;
    fileid3 fileid;
    nfstime3 atime;
    nfstime3 mtime;
    nfstime3 ctime;
};
typedef struct fattr3 fattr3;

typedef struct post_op_attr {
    bool_t attributes_follow;
    union {
        fattr3 attributes;
    } post_op_attr_u;

} post_op_attr;

enum nfs_req_result {
    NFS_REQ_OK,
    NFS_REQ_DROP,
    NFS_REQ_ERROR,
    NFS_REQ_REPLAY,
    NFS_REQ_ASYNC_WAIT,
    NFS_REQ_XPRT_DIED,
    NFS_REQ_AUTH_ERR,
};

#define FSF3_LINK 0x0001
#define FSF3_SYMLINK 0x0002
#define FSF3_HOMOGENEOUS 0x0008
#define FSF3_CANSETTIME 0x0010

#endif //DNFSD_NFS_ARGS_H

/*为结构体post_op_attr赋值*/
bool nfs_set_post_op_attr(struct stat *buf, post_op_attr *Fattr);
