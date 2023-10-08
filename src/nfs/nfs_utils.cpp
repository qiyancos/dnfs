/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Rock Lee lsk_mprc@pku.edu.cn
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
/**
 * @brief Start a duplicate request transaction
 *
 * Finds any matching request entry in the cache, if one exists, else
 * creates one in the START state.  On any non-error return, the refcnt
 * of the corresponding entry is incremented.
 *
 * @param[in] reqnfs  The NFS request data
 *
 * @retval DUPREQ_SUCCESS if successful.
 */
#include <sys/stat.h>
#include <sys/sysmacros.h>

#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "rpc/svc_auth.h"
#include "string"
using namespace std;
#define MODULE_NAME "NFS"

/*释放结果存储空间*/
void nfs_dupreq_rele(nfs_request_t *reqnfs) {

    LOG(MODULE_NAME, L_INFO, "releasing res %p",
        reqnfs->svc.rq_u2);
    reqnfs->funcdesc->free_function(
            static_cast<nfs_res_t *>(reqnfs->svc.rq_u2));
    free_nfs_res((nfs_res_t *) reqnfs->svc.rq_u2);


    if (reqnfs->svc.rq_auth)
        SVCAUTH_RELEASE(&reqnfs->svc);
}

/*为结果分配空间*/
dupreq_status_t nfs_dupreq_start(nfs_request_t *reqnfs) {
    auto *p_ = (nfs_res_t *) malloc(sizeof(nfs_res_t));
    if (nullptr == p_) {
        LOG(MODULE_NAME, L_ERROR, "Request '%s' result failed to allocate memory",
            reqnfs->funcdesc->funcname);
        return DUPREQ_DROP;
    }
    reqnfs->res_nfs = p_;
    reqnfs->svc.rq_u2 = p_;
    return DUPREQ_SUCCESS;
}

nfsstat3 nfs_set_post_op_attr(char *file_path, post_op_attr *fattr) {

    struct stat buf{};
    int stat_res = stat(file_path, &buf);
    if (stat_res != 0) {
        switch (errno) {
            case ENOENT:
                return NFS3ERR_NOENT;
            case ENOTDIR:
                return NFS3ERR_NOTDIR;
            case EACCES:
                return NFS3ERR_ACCES;
            default:
                return NFS3ERR_BADHANDLE;
        }
    }

    // type 类型
    switch (buf.st_mode & S_IFMT) {
        case S_IFDIR:
            fattr->post_op_attr_u.attributes.type = NF3DIR;
            break;
        case S_IFCHR:
            fattr->post_op_attr_u.attributes.type = NF3CHR;
            break;
        case S_IFBLK:
            fattr->post_op_attr_u.attributes.type = NF3BLK;
            break;
        case S_IFREG:
            fattr->post_op_attr_u.attributes.type = NF3REG;
            break;
        case S_IFIFO:
            fattr->post_op_attr_u.attributes.type = NF3FIFO;
            break;
        case S_IFLNK:
            fattr->post_op_attr_u.attributes.type = NF3LNK;
            break;
        case S_IFSOCK:
            fattr->post_op_attr_u.attributes.type = NF3SOCK;
            break;
        default:
            return NFS3ERR_BADHANDLE;
    }
    // 权限属性
    fattr->post_op_attr_u.attributes.mode = (buf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
    // 硬链接数
    fattr->post_op_attr_u.attributes.nlink = buf.st_nlink;
    // 归属用户ID
    fattr->post_op_attr_u.attributes.uid = buf.st_uid;
    // 归属用户组ID
    fattr->post_op_attr_u.attributes.gid = buf.st_gid;
    // 文件大小
    fattr->post_op_attr_u.attributes.size = buf.st_size;
    // 文件实际使用的磁盘空间字节数
    fattr->post_op_attr_u.attributes.used = buf.st_blocks * S_BLKSIZE;
    // 设备文件描述，仅当type=NF3BLK/NF3CHR时
    if (S_ISCHR(buf.st_mode) || S_ISBLK(buf.st_mode)) {
        fattr->post_op_attr_u.attributes.rdev.specdata1 = major(buf.st_rdev);
        fattr->post_op_attr_u.attributes.rdev.specdata2 = minor(buf.st_rdev);
    }
    // 文件系统的文件系统标识符
    nfs3_uint64 st_dev_major = major(buf.st_dev);
    nfs3_uint64 st_dev_minor = minor(buf.st_dev);
    fattr->post_op_attr_u.attributes.fsid =
            st_dev_major ^ (st_dev_minor << 32 | st_dev_minor >> 32);
    // 在文件系统中唯一标识文件的数字
    fattr->post_op_attr_u.attributes.fileid = buf.st_ino;
    // 文件内容上次访问时间
    fattr->post_op_attr_u.attributes.atime.tv_sec = buf.st_atim.tv_sec;
    fattr->post_op_attr_u.attributes.atime.tv_nsec = buf.st_atim.tv_nsec;
    // 文件内容上次修改时间
    fattr->post_op_attr_u.attributes.mtime.tv_sec = buf.st_mtim.tv_sec;
    fattr->post_op_attr_u.attributes.mtime.tv_nsec = buf.st_mtim.tv_nsec;
    // 文件属性上次修改时间
    fattr->post_op_attr_u.attributes.ctime.tv_sec = buf.st_ctim.tv_sec;
    fattr->post_op_attr_u.attributes.ctime.tv_nsec = buf.st_ctim.tv_nsec;
    fattr->attributes_follow = true;
    return NFS3_OK;
}

/*获取文件句柄*/
void get_file_handle(nfs_fh3 &request_handle) {
    /*获取句柄*/
    char *split_file=(char*)malloc(sizeof(char) * request_handle.data.data_len);
    char *head=split_file;
    u_int i=request_handle.data.data_len;
    while (i--)
    {
        *(head++) =*request_handle.data.data_val++;
    }
    *(head++)='\0';
    request_handle.data.data_val=split_file;
}
