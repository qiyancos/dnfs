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

#ifndef DNFSD_NFS_UTILS_H
#define DNFSD_NFS_UTILS_H

#include "dnfsd/dnfs_meta_data.h"

typedef enum dupreq_status {
    DUPREQ_SUCCESS = 0,
    DUPREQ_BEING_PROCESSED,
    DUPREQ_EXISTS,
    DUPREQ_DROP,
} dupreq_status_t;

/*释放请求内存*/
static inline void free_nfs_res(nfs_res_t *res) {
    free(res);
}

/*释放结果存储空间*/
void nfs_dupreq_rele(nfs_request_t *reqnfs);

/*为结果分配空间*/
dupreq_status_t nfs_dupreq_start(nfs_request_t *);


/*为结构体post_op_attr赋值*/
nfsstat3 nfs_set_post_op_attr(char *file_path, post_op_attr *Fattr);

/*获取文件句柄*/
void get_file_handle(nfs_fh3 &request_handle);

#endif //DNFSD_NFS_UTILS_H
