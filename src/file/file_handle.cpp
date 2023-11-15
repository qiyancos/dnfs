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

#include "file/file_handle.h"

/**
 *
 *  nfs3_FhandleToCache: gets the FSAL obj from the NFSv3 file handle.
 *
 * Validates and Converts a V3 file handle and then gets the FSAL obj.
 *
 * @param fh3 [IN] pointer to the file handle to be converted
 * @param status [OUT] protocol status
 * @param rc [OUT] operation status
 *
 * @return FSAL obj or NULL on failure
 *
 */
//struct fsal_obj_handle *nfs3_FhandleToCache(
//        nfs_fh3 *fh3, nfsstat3 *status, int *rc) {
//    fsal_status_t fsal_status;
//    file_handle_v3_t *v3_handle;
//    struct fsal_export *export;
//    struct fsal_obj_handle *obj = NULL;
//    struct gsh_buffdesc fh_desc;
//    char fhbuf[NFS3_FHSIZE];
//
//    /* Default behaviour */
//    *rc = NFS_REQ_OK;
//
//    /* validate the filehandle  */
//    *status = nfs3_Is_Fh_Invalid(fh3);
//
//    if (*status != NFS3_OK)
//        goto badhdl;
//
//    /* Cast the fh as a non opaque structure */
//    v3_handle = (file_handle_v3_t *) (fh3->data.data_val);
//
//    assert(ntohs(v3_handle->exportid) == op_ctx->ctx_export->export_id);
//
//    export = op_ctx->fsal_export;
//
//    /*
//     * FIXME: the wire handle can obviously be no larger than NFS4_FHSIZE,
//     * but there is no such limit on a host handle. Here, we assume that as
//     * the size limit. Eventually it might be nice to call into the FSAL to
//     * ask how large a buffer it needs for a host handle.
//     */
//    memcpy(fhbuf, &v3_handle->fsopaque, v3_handle->fs_len);
//    fh_desc.len = v3_handle->fs_len;
//    fh_desc.addr = fhbuf;
//
//    /* adjust the wire handle opaque into a host-handle */
//    fsal_status =
//            export->exp_ops.wire_to_host(export, FSAL_DIGEST_NFSV3,
//                                         &fh_desc,
//                                         v3_handle->fhflags1);
//
//    if (!FSAL_IS_ERROR(fsal_status))
//        fsal_status = export->exp_ops.create_handle(export, &fh_desc,
//                                                    &obj, NULL);
//
//    if (FSAL_IS_ERROR(fsal_status)) {
//        *status = nfs3_Errno_status(fsal_status);
//        if (nfs_RetryableError(fsal_status.major))
//            *rc = NFS_REQ_DROP;
//    }
//
//    badhdl:
//    return obj;
//}
const char *msg_fsal_err(fsal_errors_t fsal_err)
{
    switch (fsal_err) {
        case ERR_FSAL_NO_ERROR:
            return "No error";
        case ERR_FSAL_PERM:
            return "Forbidden action";
        case ERR_FSAL_NOENT:
            return "No such file or directory";
        case ERR_FSAL_IO:
            return "I/O error";
        case ERR_FSAL_NXIO:
            return "No such device or address";
        case ERR_FSAL_NOMEM:
            return "Not enough memory";
        case ERR_FSAL_ACCESS:
            return "Permission denied";
        case ERR_FSAL_FAULT:
            return "Bad address";
        case ERR_FSAL_STILL_IN_USE:
            return "Device or resource busy";
        case ERR_FSAL_EXIST:
            return "This object already exists";
        case ERR_FSAL_XDEV:
            return "This operation can't cross filesystems";
        case ERR_FSAL_NOTDIR:
            return "This object is not a directory";
        case ERR_FSAL_ISDIR:
            return "Directory used in a nondirectory operation";
        case ERR_FSAL_INVAL:
            return "Invalid object type";
        case ERR_FSAL_FBIG:
            return "File exceeds max file size";
        case ERR_FSAL_NOSPC:
            return "No space left on filesystem";
        case ERR_FSAL_ROFS:
            return "Read-only filesystem";
        case ERR_FSAL_MLINK:
            return "Too many hard links";
        case ERR_FSAL_DQUOT:
            return "Quota exceeded";
        case ERR_FSAL_NAMETOOLONG:
            return "Max name length exceeded";
        case ERR_FSAL_NOTEMPTY:
            return "The directory is not empty";
        case ERR_FSAL_STALE:
            return "The file no longer exists";
        case ERR_FSAL_BADHANDLE:
            return "Illegal filehandle";
        case ERR_FSAL_BADCOOKIE:
            return "Invalid cookie";
        case ERR_FSAL_NOTSUPP:
            return "Operation not supported";
        case ERR_FSAL_TOOSMALL:
            return "Output buffer too small";
        case ERR_FSAL_SERVERFAULT:
            return "Undefined server error";
        case ERR_FSAL_BADTYPE:
            return "Invalid type for create operation";
        case ERR_FSAL_DELAY:
            return "File busy, retry";
        case ERR_FSAL_FHEXPIRED:
            return "Filehandle expired";
        case ERR_FSAL_SYMLINK:
            return "This is a symbolic link, should be file/directory";
        case ERR_FSAL_ATTRNOTSUPP:
            return "Attribute not supported";
        case ERR_FSAL_NOT_INIT:
            return "Filesystem not initialized";
        case ERR_FSAL_ALREADY_INIT:
            return "Filesystem already initialised";
        case ERR_FSAL_BAD_INIT:
            return "Filesystem initialisation error";
        case ERR_FSAL_SEC:
            return "Security context error";
        case ERR_FSAL_NO_QUOTA:
            return "No Quota available";
        case ERR_FSAL_NOT_OPENED:
            return "File/directory not opened";
        case ERR_FSAL_DEADLOCK:
            return "Deadlock";
        case ERR_FSAL_OVERFLOW:
            return "Overflow";
        case ERR_FSAL_INTERRUPT:
            return "Operation Interrupted";
        case ERR_FSAL_BLOCKED:
            return "Lock Blocked";
        case ERR_FSAL_SHARE_DENIED:
            return "Share Denied";
        case ERR_FSAL_LOCKED:
            return "Locked";
        case ERR_FSAL_TIMEOUT:
            return "Timeout";
        case ERR_FSAL_FILE_OPEN:
            return "File Open";
        case ERR_FSAL_UNION_NOTSUPP:
            return "Union Not Supported";
        case ERR_FSAL_IN_GRACE:
            return "Server in Grace";
        case ERR_FSAL_NO_DATA:
            return "No Data";
        case ERR_FSAL_NO_ACE:
            return "No matching ACE";
        case ERR_FSAL_BAD_RANGE:
            return "Lock not in allowable range";
        case ERR_FSAL_CROSS_JUNCTION:
            return "Crossed Junction";
        case ERR_FSAL_BADNAME:
            return "Invalid Name";
        case ERR_FSAL_NOXATTR:
            return "No such xattr";
        case ERR_FSAL_XATTR2BIG:
            return "Xattr too big";
    }

    return "Unknown FSAL error";
}