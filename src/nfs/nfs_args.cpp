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

#include "nfs/nfs_args.h"

bool nfs_set_post_op_attr(struct stat *buf, post_op_attr *fattr)
{
    switch (buf->st_mode & S_IFMT)
    {
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
        return (false);
    }
    fattr->post_op_attr_u.attributes.mode = (buf->st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
    fattr->post_op_attr_u.attributes.nlink = buf->st_nlink;
    fattr->post_op_attr_u.attributes.uid = buf->st_uid;
    fattr->post_op_attr_u.attributes.gid = buf->st_gid;
    fattr->post_op_attr_u.attributes.size = buf->st_size;
    fattr->post_op_attr_u.attributes.used = buf->st_size;
    fattr->post_op_attr_u.attributes.atime.tv_sec = buf->st_atim.tv_sec;
    fattr->post_op_attr_u.attributes.atime.tv_nsec = buf->st_atim.tv_nsec;
    fattr->post_op_attr_u.attributes.mtime.tv_sec = buf->st_mtim.tv_sec;
    fattr->post_op_attr_u.attributes.mtime.tv_nsec = buf->st_mtim.tv_nsec;
    fattr->post_op_attr_u.attributes.ctime.tv_sec = buf->st_ctim.tv_sec;
    fattr->post_op_attr_u.attributes.ctime.tv_nsec = buf->st_ctim.tv_nsec;
    // todo 以下4个属性没找到获取方法rdev/fsid/fileid
    fattr->post_op_attr_u.attributes.rdev.specdata1 = 0;
    fattr->post_op_attr_u.attributes.rdev.specdata2 = 0;
    fattr->post_op_attr_u.attributes.fsid = 0;
    fattr->post_op_attr_u.attributes.fileid = 0;
    fattr->attributes_follow = true;
    return (true);
}