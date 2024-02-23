/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Rock Lee lsk_mprc@pku.edu.cn
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License; This program is
 * distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the MIT license for
 * more details. You should have received a copy of the MIT License
 * along with this project.
 *
 */

#ifndef DNFSD_NFS_UTILS_H
#define DNFSD_NFS_UTILS_H

#include <sys/syscall.h>

#include "dnfsd/dnfs_meta_data.h"
#include "utils/common_utils.h"
#include "string"

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


/* 获取文件属性
 * params file_path:获取属性文件路径
 * params Fattr:保存获取属性
 * return 是否获取成功
 * */
nfsstat3 nfs_set_post_op_attr(char *file_path, post_op_attr *Fattr);

/*获取文件句柄
 * params request_handle:请求参数句柄
 * */
void get_file_handle(nfs_fh3 &request_handle);

/*获取文件操作之前属性信息
 * params file_path:获取属性文件路径
 * params pre_attr:保存文件操作前属性
 * return 是否获取成功
 * */
nfsstat3 get_pre_op_attr(char *file_path, pre_op_attr &pre_attr);

/*判断文件是否存在
 * params file_path:判断存在的文件路径
 * params judge_mode:判断文件格式（文件,文件夹）
 * return 是否满足要求
 * */
bool judge_file_exit(const char *file_path, int judge_mode);

/*获取文件弱属性对比信息
 * params file_path:获取属性文件路径
 * params pre_attr:文件操作前属性
 * params wccData:保存文件若属性对比信息
 * return 是否获取成功
 * */
nfsstat3 get_wcc_data(char *file_path, pre_op_attr &pre_attr, wcc_data &wccData);

/*删除目录
 * params path:删除的目录
 * return 是否删除成功
 * */
bool remove_directory(const std::string &path);

/*删除文件
 * params path:删除的目录
 * return 是否删除成功
 * */
bool remove_file(const std::string &path);

/*建立文件句柄
 * params fh:建立的句柄
 * params file_path:构造参数
 * */
void set_file_handle(nfs_fh3 *fh, const std::string &file_path);


/*设置文件属性
 * params file_path:获取属性文件路径
 * params new_attr:文件新属性
 * return 是否修改成功
 * */
nfsstat3 nfs_set_sattr3(const char *file_path, sattr3 &new_attr);

/**
 * @brief Read system directory entries into the buffer
 *
 * @param[in]     fd     File descriptor of open directory
 * @param[in]     buf    The buffer
 * @param[in]     bcount Buffer size
 * @param[in,out] basepp Offset into "file" after this read
 */
int vfs_readents(int fd, char *buf, unsigned int bcount, off_t *basepp);

/**
 * @brief Mash a FreeBSD directory entry into the generic form
 *
 * @param buf  [in] pointer into buffer read by vfs_readents
 * @param bpos [in] byte offset into buf to decode
 * @param vd   [in] pointer to the generic struct
 * @param base [in] base file offset for this buffer - not used
 *
 * @return true. Linux entries are never empty.
 */
bool to_vfs_dirent(char *buf, int bpos, struct vfs_dirent *vd, off_t base);

fsal_status_t wait_to_start_io(struct f_handle *file_handele,
                               int openflags);

void update_share_counters(struct fsal_share *share,
                           fsal_openflags_t old_openflags,
                           fsal_openflags_t new_openflags);

fsal_status_t fsalstat(fsal_errors_t major, int minor);

fsal_status_t check_share_conflict(struct fsal_share *share,
                                   fsal_openflags_t openflags,
                                   bool bypass);

fsal_status_t check_share_conflict_and_update(struct fsal_share *share,
                                              fsal_openflags_t old_openflags,
                                              fsal_openflags_t new_openflags,
                                              bool bypass);

fsal_status_t check_share_conflict_and_update_locked(struct f_handle *file_handle,
                                                     struct fsal_share *share,
                                                     fsal_openflags_t old_openflags,
                                                     fsal_openflags_t new_openflags,
                                                     bool bypass);

fsal_status_t fsal_start_global_io(struct f_handle *file_handle,
                                   fsal_openflags_t openflags,
                                   bool bypass,struct fsal_share *share);

fsal_status_t fsal_start_io(struct f_handle *file_handle,
                            struct state_t *state,
                            fsal_openflags_t openflags,
                            bool bypass,struct fsal_share *share);

bool atomic_add_unless_int32_t(int32_t *var,
                               int32_t addend,
                               int32_t unless);

bool PTHREAD_MUTEX_dec_int32_t_and_lock(int32_t *var,
                                        pthread_mutex_t *lock);

fsal_status_t fsal_complete_io(struct f_handle *file_handle);

void update_share_counters_locked(struct f_handle *file_handle,
                                  fsal_openflags_t old_openflags,
                                  fsal_openflags_t new_openflags);

#endif //DNFSD_NFS_UTILS_H
