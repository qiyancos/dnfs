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

#include <sys/sysmacros.h>

#include "nfs/nfs_args.h"

bool nfs_set_post_op_attr(struct stat *buf, post_op_attr *fattr)
{
    // type 类型
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
    // 权限属性
    fattr->post_op_attr_u.attributes.mode = (buf->st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
    // 硬链接数
    fattr->post_op_attr_u.attributes.nlink = buf->st_nlink;
    // 归属用户ID
    fattr->post_op_attr_u.attributes.uid = buf->st_uid;
    // 归属用户组ID
    fattr->post_op_attr_u.attributes.gid = buf->st_gid;
    // 文件大小
    fattr->post_op_attr_u.attributes.size = buf->st_size;
    // 文件实际使用的磁盘空间字节数
    fattr->post_op_attr_u.attributes.used = buf->st_blocks * S_BLKSIZE;
    // 设备文件描述，仅当type=NF3BLK/NF3CHR时
    if (S_ISCHR(buf->st_mode) || S_ISBLK(buf->st_mode))
    {
        fattr->post_op_attr_u.attributes.rdev.specdata1 = major(buf->st_rdev);
        fattr->post_op_attr_u.attributes.rdev.specdata2 = minor(buf->st_rdev);
    }
    // 文件系统的文件系统标识符
    fattr->post_op_attr_u.attributes.fsid = buf->st_dev;
    // 在文件系统中唯一标识文件的数字
    fattr->post_op_attr_u.attributes.fileid = buf->st_ino;
    // 文件内容上次访问时间
    fattr->post_op_attr_u.attributes.atime.tv_sec = buf->st_atim.tv_sec;
    fattr->post_op_attr_u.attributes.atime.tv_nsec = buf->st_atim.tv_nsec;
    // 文件内容上次修改时间
    fattr->post_op_attr_u.attributes.mtime.tv_sec = buf->st_mtim.tv_sec;
    fattr->post_op_attr_u.attributes.mtime.tv_nsec = buf->st_mtim.tv_nsec;
    // 文件属性上次修改时间
    fattr->post_op_attr_u.attributes.ctime.tv_sec = buf->st_ctim.tv_sec;
    fattr->post_op_attr_u.attributes.ctime.tv_nsec = buf->st_ctim.tv_nsec;
    fattr->attributes_follow = true;
    return (true);
}