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
#include "file/fsal_handle.h"
#include "log/log.h"
#include "rpc/svc_auth.h"
#include "string"

using namespace std;
#define MODULE_NAME "NFS"

/*释放结果存储空间*/
void nfs_dupreq_rele(nfs_request_t *reqnfs) {

    /*每个具体接口里的free function*/
    reqnfs->funcdesc->free_function(
            static_cast<nfs_res_t *>(reqnfs->svc.rq_u2));

    LOG(MODULE_NAME, L_INFO, "releasing res %p",
        reqnfs->svc.rq_u2);
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

/* 获取文件属性
 * params file_path:获取属性文件路径
 * params Fattr:保存获取属性
 * return 是否获取成功
 * */
nfsstat3 nfs_set_post_op_attr(char *file_path, post_op_attr *fattr) {

    struct stat buf{};
    int stat_res = lstat(file_path, &buf);
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
        case S_IFIFO:
            fattr->post_op_attr_u.attributes.type = NF3FIFO;
            break;
        case S_IFCHR:
            fattr->post_op_attr_u.attributes.type = NF3CHR;
            break;
        case S_IFDIR:
            fattr->post_op_attr_u.attributes.type = NF3DIR;
            break;
        case S_IFBLK:
            fattr->post_op_attr_u.attributes.type = NF3BLK;
            break;
        case S_IFREG:
        case S_IFMT:
            fattr->post_op_attr_u.attributes.type = NF3REG;
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

/*获取文件句柄
 * params request_handle:请求参数句柄
 * */
void get_file_handle(nfs_fh3 &request_handle) {
    /*获取句柄*/
    string save_handle = request_handle.data.data_val;
    /*释放之前的空间*/
    free(request_handle.data.data_val);
    /*重新申请内存*/
    request_handle.data.data_val = (char *) gsh_calloc(request_handle.data.data_len + 1,
                                                       sizeof(char));
    /*截取句柄*/
    memcpy(request_handle.data.data_val, save_handle.c_str(),
           request_handle.data.data_len);
    /*添加结束符*/
    *(request_handle.data.data_val + request_handle.data.data_len) = '\0';
}

/*获取文件操作之前属性信息
 * params file_path:获取属性文件路径
 * params pre_attr:保存文件操作前属性
 * return 是否获取成功
 * */
nfsstat3 get_pre_op_attr(char *file_path, pre_op_attr &pre_attr) {
    struct post_op_attr get_a{};
    nfsstat3 status = nfs_set_post_op_attr(
            file_path, &get_a);
    if (status != NFS3_OK) {
        return status;
    }

    pre_attr.attributes_follow = true;

    /*添加属性*/
    pre_attr.pre_op_attr_u.attributes.size = get_a.post_op_attr_u.attributes.size;
    pre_attr.pre_op_attr_u.attributes.ctime.tv_sec = get_a.post_op_attr_u.attributes.ctime.tv_sec;
    pre_attr.pre_op_attr_u.attributes.ctime.tv_nsec = get_a.post_op_attr_u.attributes.ctime.tv_nsec;
    pre_attr.pre_op_attr_u.attributes.mtime.tv_sec = get_a.post_op_attr_u.attributes.mtime.tv_sec;
    pre_attr.pre_op_attr_u.attributes.mtime.tv_nsec = get_a.post_op_attr_u.attributes.mtime.tv_nsec;
    return NFS3_OK;
}

/*判断文件是否存在
 * params file_path:判断存在的文件路径
 * params judge_mode:判断文件格式（文件,文件夹）
 * return 是否满足要求
 * */
bool judge_file_exit(const char *file_path, int judge_mode) {
    struct stat info{};
    /*不存在直接返回*/
    if (lstat(file_path, &info) != 0) {
        LOG(MODULE_NAME, D_INFO, "judge_file_exit lstat false, filepath: %s", file_path);
        return false;
    } else if (!(info.st_mode & judge_mode)) {
        /*存在但不是需要的文件格式*/
        LOG(MODULE_NAME, D_INFO,
            "judge_file_exit judge false, info.st_mode: %o, judge_mode: %o", info.st_mode,
            judge_mode);
        return false;
    }
    return true;
}

/*获取文件弱属性对比信息
 * params file_path:获取属性文件路径
 * params pre_attr:文件操作前属性
 * params wccData:保存文件若属性对比信息
 * return 是否获取成功
 * */
nfsstat3 get_wcc_data(char *file_path, pre_op_attr &pre_attr, wcc_data &wccData) {
    struct post_op_attr get_a{};
    nfsstat3 status = nfs_set_post_op_attr(
            file_path, &get_a);
    if (status != NFS3_OK) {
        return status;
    }
    wccData.before = pre_attr;
    wccData.after = get_a;
    return NFS3_OK;
}

/*删除目录
 * params path:删除的目录
 * return 是否删除成功
 * */
bool remove_directory(const std::string &path) {
    // 删除当前目录
    if (rmdir(path.c_str()) != 0) {
        return false;
    }

    return true;
}

/*删除文件
 * params path:删除的目录
 * return 是否删除成功
 * */
bool remove_file(const std::string &path) {

    // 如果是文件，直接删除
    if (remove(path.c_str()) != 0) {
        return false;
    }
    return true;

}

/*建立文件句柄
 * params fh:建立的句柄
 * params file_path:构造参数
 * */
void set_file_handle(nfs_fh3 *fh, const std::string &file_path) {
    /*获取句柄长度*/
    fh->data.data_len = file_path.length();

    /*为句柄申请内存*/
    fh->data.data_val = (char *) gsh_calloc(fh->data.data_len + 1, sizeof(char));

    if (fh->data.data_val == nullptr) {
        abort();
    }

    /*获取句柄内容*/
    memcpy(fh->data.data_val, file_path.c_str(), fh->data.data_len);

    /*添加结束符*/
    *(fh->data.data_val + fh->data.data_len) = '\0';
}

/*设置文件属性
 * params file_path:获取属性文件路径
 * params new_attr:文件新属性
 * return 是否修改成功
 * */
nfsstat3 nfs_set_sattr3(const char *file_path, sattr3 &new_attr) {
    struct timespec ts[2];
    struct timeval tv[2];
    bool set_time_flag = false;
    int utimes_res = -1;
    /*chmod*/
    if (new_attr.mode.set_it) {
        int chmod_res = chmod(file_path, new_attr.mode.set_mode3_u.mode);
        if (chmod_res != 0) {
            LOG(MODULE_NAME, D_ERROR,
                "Interface nfs_setattr failed to chmod '%s'",
                file_path);
            return NFS3ERR_INVAL;
        }
    }
    /*chown*/
    if (new_attr.gid.set_it || new_attr.uid.set_it) {
        gid3 new_gid = new_attr.gid.set_it ? new_attr.gid.set_gid3_u.gid : -1;
        uid3 new_uid = new_attr.uid.set_it ? new_attr.uid.set_uid3_u.uid : -1;
        int chown_res = chown(file_path, new_uid, new_gid);
        if (chown_res != 0) {
            LOG(MODULE_NAME, D_ERROR,
                "Interface nfs_setattr failed to chown '%s'",
                file_path);
            return NFS3ERR_INVAL;
        }
    }

    /*utimes*/
    if (new_attr.atime.set_it != DONT_CHANGE) {
        set_time_flag = true;
        LOG(MODULE_NAME, D_INFO, "Interface nfs_setattr atime set=%d atime = %d,%d",
            new_attr.atime.set_it,
            new_attr.atime.set_atime_u.atime.tv_sec,
            new_attr.atime.set_atime_u.atime.tv_nsec);
        if (new_attr.atime.set_it == SET_TO_CLIENT_TIME) {
            ts[0].tv_sec = new_attr.atime.set_atime_u.atime.tv_sec;
            ts[0].tv_nsec = new_attr.atime.set_atime_u.atime.tv_nsec;
        } else if (new_attr.atime.set_it == SET_TO_SERVER_TIME) {
            /* Use the server's current time */
            LOG(MODULE_NAME, D_INFO, "SET_TO_SERVER_TIME atime");
            ts[0].tv_sec = 0;
            ts[0].tv_nsec = UTIME_NOW;
        } else {
            LOG(MODULE_NAME, D_ERROR,
                "Unexpected value for sattr->atime.set_it = %d",
                new_attr.atime.set_it);
            return NFS3ERR_INVAL;
        }
    }

    if (new_attr.mtime.set_it != DONT_CHANGE) {
        set_time_flag = true;
        LOG(MODULE_NAME, D_INFO, "Interface nfs_setattr mtime set=%d mtime = %d",
            new_attr.atime.set_it,
            new_attr.mtime.set_mtime_u.mtime.tv_sec);
        if (new_attr.mtime.set_it == SET_TO_CLIENT_TIME) {
            ts[1].tv_sec = new_attr.mtime.set_mtime_u.mtime.tv_sec;
            ts[1].tv_nsec = new_attr.mtime.set_mtime_u.mtime.tv_nsec;
        } else if (new_attr.mtime.set_it == SET_TO_SERVER_TIME) {
            /* Use the server's current time */
            LOG(MODULE_NAME, D_INFO, "SET_TO_SERVER_TIME Mtime");
            ts[1].tv_sec = 0;
            ts[1].tv_nsec = UTIME_NOW;
        } else {
            LOG(MODULE_NAME, D_ERROR,
                "Unexpected value for sattr->mtime.set_it = %d",
                new_attr.mtime.set_it);
            return NFS3ERR_INVAL;
        }
    }
    if (set_time_flag) {
        if (ts[0].tv_nsec == UTIME_NOW || ts[1].tv_nsec == UTIME_NOW) {
            /* set to the current timestamp. achieve this by passing NULL timeval to kernel */
            utimes_res = utimes(file_path, nullptr);
        } else {
            TIMESPEC_TO_TIMEVAL(&tv[0], &ts[0]);
            TIMESPEC_TO_TIMEVAL(&tv[1], &ts[1]);
            utimes_res = utimes(file_path, tv);
        }
        if (utimes_res != 0) {
            LOG(MODULE_NAME, D_ERROR, "modify times failed");
            return NFS3ERR_INVAL;
        }
    }
    return NFS3_OK;
}

int vfs_readents(int fd, char *buf, unsigned int bcount, off_t *basepp) {
    int retval = 0;

    retval = syscall(SYS_getdents64, fd, buf, bcount);
    if (retval >= 0)
        *basepp += retval;
    return retval;
}

bool to_vfs_dirent(char *buf, int bpos, struct vfs_dirent *vd, off_t base) {
    auto *dp = (struct dirent64 *) (buf + bpos);
    char type;

    vd->vd_ino = dp->d_ino;
    vd->vd_reclen = dp->d_reclen;
    type = buf[dp->d_reclen - 1];
    vd->vd_type = type;
    vd->vd_offset = dp->d_off;
    vd->vd_name = dp->d_name;
    return true;
}


fsal_status_t wait_to_start_io(struct f_handle *file_handele,
                               int openflags) {
    fsal_status_t status = {ERR_FSAL_NO_ERROR, 0};

    /* Indicate we want to do I/O work */
    atomic_inc_int32_t(&file_handele->io_work);

//    if (openflags & FSAL_O_READ) {
//        /* Indicate we want to read */
//        atomic_inc_int32_t(&file_handele->want_read);
//    }
//
//    if (openflags & FSAL_O_WRITE) {
//        /* Indicate we want to read */
//        atomic_inc_int32_t(&file_handele->want_write);
//    }


//    if (openflags & FSAL_O_READ) {
//        /* Indicate we want to read */
//        atomic_dec_int32_t(&file_handele->want_read);
//    }
//
//    if (openflags & FSAL_O_WRITE) {
//        /* Indicate we want to read */
//        atomic_dec_int32_t(&file_handele->want_write);
//    }

    return status;
}

void update_share_counters(struct fsal_share *share,
                           fsal_openflags_t old_openflags,
                           fsal_openflags_t new_openflags) {
    int access_read_inc =
            ((int) (new_openflags & FSAL_O_READ) != 0) -
            ((int) (old_openflags & FSAL_O_READ) != 0);

    int access_write_inc =
            ((int) (new_openflags & FSAL_O_WRITE) != 0) -
            ((int) (old_openflags & FSAL_O_WRITE) != 0);

    int deny_read_inc =
            ((int) (new_openflags & FSAL_O_DENY_READ) != 0) -
            ((int) (old_openflags & FSAL_O_DENY_READ) != 0);

    /* Combine both FSAL_O_DENY_WRITE and FSAL_O_DENY_WRITE_MAND */
    int deny_write_inc =
            ((int) (new_openflags & FSAL_O_DENY_WRITE) != 0) -
            ((int) (old_openflags & FSAL_O_DENY_WRITE) != 0) +
            ((int) (new_openflags & FSAL_O_DENY_WRITE_MAND) != 0) -
            ((int) (old_openflags & FSAL_O_DENY_WRITE_MAND) != 0);

    int deny_write_mand_inc =
            ((int) (new_openflags & FSAL_O_DENY_WRITE_MAND) != 0) -
            ((int) (old_openflags & FSAL_O_DENY_WRITE_MAND) != 0);

    share->share_access_read += access_read_inc;
    share->share_access_write += access_write_inc;
    share->share_deny_read += deny_read_inc;
    share->share_deny_write += deny_write_inc;
    share->share_deny_write_mand += deny_write_mand_inc;

    LOG(MODULE_NAME, D_INFO,
        "share counter: access_read %u, access_write %u, deny_read %u, deny_write %u, deny_write_v4 %u",
        share->share_access_read,
        share->share_access_write,
        share->share_deny_read,
        share->share_deny_write,
        share->share_deny_write_mand);
}

fsal_status_t fsalstat(fsal_errors_t major, int minor) {
    fsal_status_t status = {major, minor};
    return status;
}

fsal_status_t check_share_conflict(struct fsal_share *share,
                                   fsal_openflags_t openflags,
                                   bool bypass) {
    string cause;

    if ((openflags & FSAL_O_READ) != 0
        && share->share_deny_read > 0
        && !bypass) {
        cause = "access read denied by existing deny read";
        goto out_conflict;
    }

    if ((openflags & FSAL_O_WRITE) != 0
        && (share->share_deny_write_mand > 0 ||
            (!bypass && share->share_deny_write > 0))) {
        cause = "access write denied by existing deny write";
        goto out_conflict;
    }

    if ((openflags & FSAL_O_DENY_READ) != 0
        && share->share_access_read > 0) {
        cause = "deny read denied by existing access read";
        goto out_conflict;
    }

    if (((openflags & FSAL_O_DENY_WRITE) != 0 ||
         (openflags & FSAL_O_DENY_WRITE_MAND) != 0)
        && share->share_access_write > 0) {
        cause = "deny write denied by existing access write";
        goto out_conflict;
    }

    return fsalstat(ERR_FSAL_NO_ERROR, 0);

    out_conflict:

    LOG(MODULE_NAME, D_INFO,
        "Share conflict detected: %s openflags=%d bypass=%s",
        cause.c_str(), (int) openflags,
        bypass ? "yes" : "no");

    LOG(MODULE_NAME, D_INFO,
        "share->share_deny_read=%d share->share_deny_write=%d share->share_access_read=%d share->share_access_write=%d",
        share->share_deny_read, share->share_deny_write,
        share->share_access_read, share->share_access_write);

    return fsalstat(ERR_FSAL_SHARE_DENIED, 0);
}

fsal_status_t check_share_conflict_and_update(struct fsal_share *share,
                                              fsal_openflags_t old_openflags,
                                              fsal_openflags_t new_openflags,
                                              bool bypass) {
    fsal_status_t status;

    status = check_share_conflict(share, new_openflags, bypass);

    if (!FSAL_IS_ERROR(status)) {
        /* Take the share reservation now by updating the counters. */
        update_share_counters(share, old_openflags, new_openflags);
    }

    return status;
}


fsal_status_t check_share_conflict_and_update_locked(struct f_handle *file_handle,
                                                     struct fsal_share *share,
                                                     fsal_openflags_t old_openflags,
                                                     fsal_openflags_t new_openflags,
                                                     bool bypass) {
    fsal_status_t status;

    FsalHandle::pthread_lock_write(&file_handle->handle_rwlock_lock);

    status = check_share_conflict_and_update(share, old_openflags,
                                             new_openflags, bypass);

    FsalHandle::pthread_unlock_rw(&file_handle->handle_rwlock_lock);

    return status;
}

fsal_status_t fsal_start_global_io(struct f_handle *file_handle,
                                   fsal_openflags_t openflags,
                                   bool bypass, struct fsal_share *share) {
    fsal_status_t status = {ERR_FSAL_NO_ERROR, 0};
    bool open_any = openflags == FSAL_O_ANY;

    if (!open_any && share != nullptr) {
        status = check_share_conflict_and_update_locked(file_handle,
                                                        share,
                                                        FSAL_O_CLOSED,
                                                        openflags,
                                                        bypass);

        if (FSAL_IS_ERROR(status)) {
            FsalHandle::pthread_unlock_rw(&file_handle->handle_rwlock_lock);
            LOG(MODULE_NAME, D_ERROR,
                "check_share_conflict_and_update_locked failed with %s",
                fsal_err_txt(status));
            return status;
        }
    }

    status = wait_to_start_io(file_handle, openflags);

    return status;
}

fsal_status_t fsal_start_io(struct f_handle *file_handle,
                            struct state_t *state,
                            fsal_openflags_t openflags,
                            bool bypass, struct fsal_share *share) {
    if (state == nullptr)
        goto global;
    wait_to_start_io(file_handle, openflags);

    global:
    return fsal_start_global_io(file_handle, openflags,
                                bypass, state == nullptr ? share : nullptr);
}

bool atomic_add_unless_int32_t(int32_t *var,
                               int32_t addend,
                               int32_t unless) {
    int32_t cur, newv;
    bool changed;

    cur = atomic_fetch_int32_t(var);
    do {
        if (cur == unless)
            return false;
        newv = cur + addend;
        changed = __atomic_compare_exchange_n(var, &cur, newv, false,
                                              __ATOMIC_SEQ_CST,
                                              __ATOMIC_SEQ_CST);
    } while (!changed);
    return true;
}

bool PTHREAD_MUTEX_dec_int32_t_and_lock(int32_t *var,
                                        pthread_mutex_t *lock) {
    if (atomic_add_unless_int32_t(var, -1, 1))
        return false;

    FsalHandle::pthread_w_mutex_lock(lock);
    if (atomic_add_int32_t(var, -1) == 0)
        return true;
    FsalHandle::pthread_w_mutex_unlock(lock);
    return false;
}

fsal_status_t fsal_complete_io(struct f_handle *file_handle) {
    fsal_status_t status = {ERR_FSAL_NO_ERROR, 0};
    bool got_mutex;

    LOG(MODULE_NAME, D_INFO,
        "%p done io_work (-1) = %i fd_work = %i",
        file_handle,
        atomic_fetch_int32_t(&file_handle->io_work) - 1,
        atomic_fetch_int32_t(&file_handle->fd_work));

    got_mutex = PTHREAD_MUTEX_dec_int32_t_and_lock(&file_handle->io_work,
                                                   &file_handle->work_mutex);

//    if (got_mutex)
//        FsalHandle::pthread_w_cond_signal(&file_handle->work_cond);
//
    if (got_mutex)
        FsalHandle::pthread_w_mutex_unlock(&file_handle->work_mutex);

    return status;
}

void update_share_counters_locked(struct f_handle *file_handle,
                                  fsal_openflags_t old_openflags,
                                  fsal_openflags_t new_openflags) {
    FsalHandle::pthread_lock_write(&file_handle->handle_rwlock_lock);

    update_share_counters(file_handle->share, old_openflags, new_openflags);

    FsalHandle::pthread_unlock_rw(&file_handle->handle_rwlock_lock);
}