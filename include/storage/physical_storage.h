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
#ifndef DNFSD_PHYSICAL_STORAGE_H
#define DNFSD_PHYSICAL_STORAGE_H

#include "netinet/in.h"
#include "storage_band/protocol_base.h"
#include "file/fsal_handle.h"

/*存储操作错误码*/
enum storage_errors {
    ERR_NO_ERROR = 0,
};

/*文件类型*/
enum object_file_type {
    NO_FILE_TYPE = 0,    /* sanity check to ignore type */
    REGULAR_FILE = 1,
    CHARACTER_FILE = 2,
    BLOCK_FILE = 3,
    SYMBOLIC_LINK = 4,
    SOCKET_FILE = 5,
    FIFO_FILE = 6,
    DIRECTORY = 7,
    EXTENDED_ATTR = 8
};


class PhysicalStorage {
private:
    /*所属节点*/
    sockaddr_in sock_addr;
    /*全部空间大小*/
    uint64_t total_size;
    /*以用空间大小*/
    uint64_t used_size;
public:
    /*查询文件
     *params dir_handle:查找目录句柄
     *params path:查找的文件
     *params ...:其他待补充的参数
     * return 操作码
     * */
    virtual storage_errors lookup(struct f_handle *dir_handle,
                                  const char *path, ...);

    /*读取文件目录
     *params dir_handle:读取目录句柄
     *params ...:其他待补充的参数
     * return 操作码
     * */
    virtual storage_errors readdir(struct f_handle *dir_handle, ...);

    /*创建目录
     *params dir_handle:父目录句柄
     *params name:创建的目录名
     *params ...:其他待补充的参数
     * return 操作码
     * */
    virtual storage_errors readdir(struct f_handle *dir_handle, char *name, ...);

    /*创建特殊文件
     *params file_handle:父目录句柄
     *params name:创建的文件名
     *params node_type:创建的文件类型
     *params ...:其他待补充的参数
     * return 操作码
     * */
    virtual storage_errors
    mknode(struct f_handle *file_handle, char *name, object_file_type node_type, ...);

    /*创建软连接
     *params file_handle:父目录句柄
     *params name:创建的连接名
     *params link_path:创建软连接的路径
     *params ...:其他待补充的参数
     * return 操作码
     * */
    virtual storage_errors symlink(struct f_handle *dir_hdl,
                                   const char *name,
                                   const char *link_path, ...);

    /*读取链接文件
     *params file_handle:读取链接句柄
     *params ...:其他待补充的参数
     * return 操作码
     * */
    virtual storage_errors read_link(struct f_handle *file_handle, ...);

    /*获取文件属性
     *params file_handle:获取属性的文件句柄
     *params ...:其他待补充的参数
     * return 操作码
     * */
    virtual storage_errors get_attrs(struct f_handle *file_handle, ...);

    /*创建硬链接
     *params file_handle:链接的文件句柄
     *params destdir_hdl:创建链接的目录句柄
     *params name:创建的连接名
     *params ...:其他待补充的参数
     * return 操作码
     * */
    virtual storage_errors link(struct f_handle *file_handle,
                                struct f_handle *destdir_hdl,
                                const char *name, ...);

    /*重命名文件
     * params obj_hdl:重命名的文件句柄
     * params olddir_hdl:重命名的文件所在目录句柄
     * params old_name:旧文件名
     * params newdir_hdl:重命名文件新的目录句柄
     * params new_name:新文件名
     * return 操作码
     * */
    virtual storage_errors rename(struct f_handle *obj_hdl,
                                  struct f_handle *olddir_hdl,
                                  const char *old_name,
                                  struct f_handle *newdir_hdl,
                                  const char *new_name);

    /**/

};


#endif //DNFSD_PHYSICAL_STORAGE_H
