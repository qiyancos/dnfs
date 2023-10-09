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
extern "C"
{
#include "rpc/svc.h"
}

#include "file/file_handle.h"

#define NFS3_FHSIZE 64

#define XDR_STRING_MAXLEN (8*1024)

typedef int32_t bool_t;

typedef uint32_t nfs3_uint32;

typedef uint64_t nfs3_uint64;

typedef nfs3_uint32 uid3;

typedef nfs3_uint32 gid3;

typedef nfs3_uint32 mode3;

typedef nfs3_uint64 size3;

typedef nfs3_uint64 fileid3;

typedef char *filename3;

typedef nfs3_uint64 cookie3;

typedef char cookieverf3[8];

typedef nfs3_uint32 count3;

typedef char *filename3;

typedef char *nfspath3;

struct nfstime3 {

    nfs3_uint32 tv_sec;
    nfs3_uint32 tv_nsec;
};

enum nfsstat3 {
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
};

enum ftype3 {
    NF3REG = 1,
    NF3DIR = 2,
    NF3BLK = 3,
    NF3CHR = 4,
    NF3LNK = 5,
    NF3SOCK = 6,
    NF3FIFO = 7
};

struct specdata3 {
    nfs3_uint32 specdata1;
    nfs3_uint32 specdata2;
};

struct fattr3 {
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

struct post_op_attr {
    bool_t attributes_follow;
    union {
        fattr3 attributes;
    } post_op_attr_u;

};

struct post_op_fh3 {
    bool_t handle_follows;
    union {
        nfs_fh3 handle;
    } post_op_fh3_u;
};

enum nfs_req_result {
    NFS_REQ_OK,
    NFS_REQ_DROP,
    NFS_REQ_ERROR,
    NFS_REQ_REPLAY,
    NFS_REQ_ASYNC_WAIT,
    NFS_REQ_XPRT_DIED,
    NFS_REQ_AUTH_ERR,
};

struct diropargs3 {
    nfs_fh3 dir;
    filename3 name;
};

struct wcc_attr {
    size3 size;
    nfstime3 mtime;
    nfstime3 ctime;
};

struct pre_op_attr {
    bool_t attributes_follow;
    union {
        wcc_attr attributes;
    } pre_op_attr_u;
};

struct wcc_data {
    pre_op_attr before;
    post_op_attr after;
};

struct set_mode3 {
    bool_t set_it;
    union {
        mode3 mode;
    } set_mode3_u;
};

struct set_uid3 {
    bool_t set_it;
    union {
        uid3 uid;
    } set_uid3_u;
};

struct set_gid3 {
    bool_t set_it;
    union {
        gid3 gid;
    } set_gid3_u;
};

struct set_size3 {
    bool_t set_it;
    union {
        size3 size;
    } set_size3_u;
};

enum time_how {
    DONT_CHANGE = 0,
    SET_TO_SERVER_TIME = 1,
    SET_TO_CLIENT_TIME = 2
};

struct set_atime {
    time_how set_it;
    union {
        nfstime3 atime;
    } set_atime_u;
};

struct set_mtime {
    time_how set_it;
    union {
        nfstime3 mtime;
    } set_mtime_u;
};

struct sattr3 {
    set_mode3 mode;
    set_uid3 uid;
    set_gid3 gid;
    set_size3 size;
    set_atime atime;
    set_mtime mtime;
};

#define FSF3_LINK 0x0001
#define FSF3_SYMLINK 0x0002
#define FSF3_HOMOGENEOUS 0x0008
#define FSF3_CANSETTIME 0x0010

#endif // DNFSD_NFS_ARGS_H
