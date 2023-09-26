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
#include <sys/stat.h>

#include "nfs/nfs_base.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"
#include "nfs/nfs_fsinfo.h"

#define MODULE_NAME "NFS"
int nfs3_fsinfo(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res)
{
    int rc = NFS_REQ_OK;
    FSINFO3resok *const FSINFO_FIELD = &res->res_fsinfo3.FSINFO3res_u.resok;

    LOG(MODULE_NAME, L_INFO, "The value of the obtained file handle is '%s', and the length is data_val is '%d'",
        arg->arg_fsinfo3.fsroot.data.data_val,
        arg->arg_fsinfo3.fsroot.data.data_len);

    /* To avoid setting it on each error case */
    res->res_fsinfo3.FSINFO3res_u.resfail.obj_attributes.attributes_follow =
        FALSE;

    struct stat buf;
    // u_int len = arg->arg_fsinfo3.fsroot.data.data_len;
    // char *filepath = (char *)malloc(sizeof(char) * (len + 1));
    // char *dst = filepath;
    // char *src = arg->arg_fsinfo3.fsroot.data.data_val;
    // while (len--)
    //     *(dst++) = *(src++);
    // *(dst++) = '\0';
    // int stat_res = stat(filepath, &buf);
    int stat_res = stat(arg->arg_fsinfo3.fsroot.data.data_val, &buf);
    if (stat_res != 0)
    {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR, "'stat %s' failed, errno: %d",
            arg->arg_fsinfo3.fsroot.data.data_val, errno);
        switch (errno)
        {
        case ENOENT:
            res->res_fsinfo3.status = NFS3ERR_NOENT;
            break;
        case ENOTDIR:
            res->res_fsinfo3.status = NFS3ERR_NOTDIR;
            break;
        case EACCES:
            res->res_fsinfo3.status = NFS3ERR_ACCES;
            break;
        default:
            res->res_fsinfo3.status = NFS3ERR_BADHANDLE;
            break;
        }
        goto out;
    }

    FSINFO_FIELD->obj_attributes.attributes_follow = TRUE;
    switch (buf.st_mode & S_IFMT)
    {
    case S_IFDIR:
        FSINFO_FIELD->obj_attributes.post_op_attr_u.attributes.type = DIRECTORY;
        break;
    case S_IFCHR:
        FSINFO_FIELD->obj_attributes.post_op_attr_u.attributes.type = CHARACTER_FILE;
        break;
    case S_IFBLK:
        FSINFO_FIELD->obj_attributes.post_op_attr_u.attributes.type = BLOCK_FILE;
        break;
    case S_IFREG:
        FSINFO_FIELD->obj_attributes.post_op_attr_u.attributes.type = REGULAR_FILE;
        break;
    case S_IFIFO:
        FSINFO_FIELD->obj_attributes.post_op_attr_u.attributes.type = FIFO_FILE;
        break;
    case S_IFLNK:
        FSINFO_FIELD->obj_attributes.post_op_attr_u.attributes.type = SYMBOLIC_LINK;
        break;
    case S_IFSOCK:
        FSINFO_FIELD->obj_attributes.post_op_attr_u.attributes.type = SOCKET_FILE;
        break;
    default:
        FSINFO_FIELD->obj_attributes.post_op_attr_u.attributes.type = NO_FILE_TYPE;
        break;
    }
    FSINFO_FIELD->obj_attributes.post_op_attr_u.attributes.mode = (buf.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
    FSINFO_FIELD->obj_attributes.post_op_attr_u.attributes.numlinks = buf.st_nlink;
    FSINFO_FIELD->obj_attributes.post_op_attr_u.attributes.owner = buf.st_uid;
    FSINFO_FIELD->obj_attributes.post_op_attr_u.attributes.group = buf.st_gid;
    FSINFO_FIELD->obj_attributes.post_op_attr_u.attributes.filesize = buf.st_size;
    // FSINFO_FIELD->obj_attributes.post_op_attr_u.attributes.spaceused = NULL;
    // FSINFO_FIELD->obj_attributes.post_op_attr_u.attributes.rawdev.major = NULL;
    // FSINFO_FIELD->obj_attributes.post_op_attr_u.attributes.rawdev.minor = NULL;
    // FSINFO_FIELD->obj_attributes.post_op_attr_u.attributes.fsid3 = NULL;
    // FSINFO_FIELD->obj_attributes.post_op_attr_u.attributes.fileid = NULL;
    FSINFO_FIELD->obj_attributes.post_op_attr_u.attributes.atime.tv_sec = buf.st_atim.tv_sec;
    FSINFO_FIELD->obj_attributes.post_op_attr_u.attributes.atime.tv_nsec = buf.st_atim.tv_nsec;
    FSINFO_FIELD->obj_attributes.post_op_attr_u.attributes.mtime.tv_sec = buf.st_mtim.tv_sec;
    FSINFO_FIELD->obj_attributes.post_op_attr_u.attributes.mtime.tv_nsec = buf.st_mtim.tv_nsec;
    FSINFO_FIELD->obj_attributes.post_op_attr_u.attributes.ctime.tv_sec = buf.st_ctim.tv_sec;
    FSINFO_FIELD->obj_attributes.post_op_attr_u.attributes.ctime.tv_nsec = buf.st_ctim.tv_nsec;
    //
    //    obj = nfs3_FhandleToCache(&arg->arg_fsinfo3.fsroot,
    //                              &res->res_fsinfo3.status,
    //                              &rc);

    //    if (obj == nullptr) {
    //        /* Status and rc have been set by nfs3_FhandleToCache */
    //        goto out;
    //    }
    //
    //    /* Get statistics and convert from FSAL to get time_delta */
    //    fsal_status = fsal_statfs(obj, &dynamicinfo);
    //
    //    if (FSAL_IS_ERROR(fsal_status)) {
    //        /* At this point we met an error */
    //        LogFullDebug(COMPONENT_NFSPROTO,
    //                     "failed statfs: fsal_status=%s",
    //                     fsal_err_txt(fsal_status));
    //
    //        if (nfs_RetryableError(fsal_status.major)) {
    //            /* Drop retryable errors. */
    //            rc = NFS_REQ_DROP;
    //        } else {
    //            res->res_fsstat3.status =
    //                    nfs3_Errno_status(fsal_status);
    //            rc = NFS_REQ_OK;
    //        }
    //
    //        goto out;
    //    }
    //
    //    /* New fields were added to nfs_config_t to handle this
    //       value. We use them */
    //
    FSINFO_FIELD->rtmax = 125;
    FSINFO_FIELD->rtpref = 152;
    /* This field is generally unused, it will be removed in V4 */
    FSINFO_FIELD->rtmult = DEV_BSIZE;

    FSINFO_FIELD->wtmax = 55;
    FSINFO_FIELD->wtpref = 22;
    /* This field is generally unused, it will be removed in V4 */
    FSINFO_FIELD->wtmult = DEV_BSIZE;

    FSINFO_FIELD->dtpref = 33;
    FSINFO_FIELD->maxfilesize = 44;
    FSINFO_FIELD->time_delta.tv_sec = 12;
    FSINFO_FIELD->time_delta.tv_nsec = 23;
    //
    //    LogFullDebug(COMPONENT_NFSPROTO,
    //                 "rtmax = %d | rtpref = %d | trmult = %d",
    //                 FSINFO_FIELD->rtmax, FSINFO_FIELD->rtpref,
    //                 FSINFO_FIELD->rtmult);
    //    LogFullDebug(COMPONENT_NFSPROTO,
    //                 "wtmax = %d | wtpref = %d | wrmult = %d",
    //                 FSINFO_FIELD->wtmax, FSINFO_FIELD->wtpref,
    //                 FSINFO_FIELD->wtmult);
    //    LogFullDebug(COMPONENT_NFSPROTO, "dtpref = %d | maxfilesize = %"PRIu64,
    //                 FSINFO_FIELD->dtpref, FSINFO_FIELD->maxfilesize);
    //
    //    /* Allow all kinds of operations to be performed on the server
    //       through NFS v3 */
    FSINFO_FIELD->properties =
        FSF3_LINK | FSF3_SYMLINK | FSF3_HOMOGENEOUS | FSF3_CANSETTIME;
    //
    //    nfs_SetPostOpAttr(obj,
    //                      &res->res_fsinfo3.FSINFO3res_u.resok.obj_attributes,
    //                      NULL);
    res->res_fsinfo3.status = NFS3_OK;
    //
out:
    // if (obj)
    //     obj->obj_ops->put_ref(obj);

    return rc;
} /* nfs3_fsinfo */

/**
 * @brief Free the result structure allocated for nfs3_fsinfo.
 *
 * This function frees the result structure allocated for nfs3_fsinfo.
 *
 * @param[in,out] res The result structure
 *
 */
void nfs3_fsinfo_free(nfs_res_t *res)
{
    /* Nothing to do here */
}
