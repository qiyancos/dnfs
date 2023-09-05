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