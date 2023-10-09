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
#include "nfs/nfs_read.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

int nfs3_read(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res) {

    int rc = NFS_REQ_OK;

    if (arg->arg_read3.file.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "arg_read get file handle len is 0");
        goto out;
    }
    
    
    get_file_handle(arg->arg_read3.file);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_read obtained file handle is '%s', and the length is '%d'",
        arg->arg_read3.file.data.data_val,
        arg->arg_read3.file.data.data_len);
    
    out:

    return rc;
}

void nfs3_read_free(nfs_res_t *res)
{
//    if ((res->res_read3.status == NFS3_OK)
//        && (res->res_read3.READ3res_u.resok.data.data_len != 0)) {
//        free(res->res_read3.READ3res_u.resok.data.data_val);
//    }
}

bool xdr_READ3args(XDR *xdrs, READ3args *objp) {
    struct nfs_request_lookahead *lkhd =
            xdrs->x_public ? (struct nfs_request_lookahead *) xdrs->
                    x_public : &dummy_lookahead;

    if (!xdr_nfs_fh3(xdrs, &objp->file))
        return (false);
    if (!xdr_offset3(xdrs, &objp->offset))
        return (false);
    if (!xdr_count3(xdrs, &objp->count))
        return (false);
    lkhd->flags = NFS_LOOKAHEAD_READ;
    (lkhd->read)++;
    return (true);
}

void xdr_READ3res_uio_release(struct xdr_uio *uio, u_int flags) {
    int ix;

    LOG(MODULE_NAME, D_INFO,
        "Releasing %p, references %d, count %d",
        uio, uio->uio_references, (int) uio->uio_count);

    if (!(--uio->uio_references)) {
        for (ix = 0; ix < uio->uio_count; ix++) {
            free(uio->uio_vio[ix].vio_base);
        }
        free(uio);
    }
}

struct xdr_uio *xdr_READ3res_uio_setup(struct READ3resok *objp) {
    struct xdr_uio *uio;
    u_int size = objp->data.data_len;
    /* The size to actually be written must be a multiple of
     * BYTES_PER_XDR_UNIT
     */
    u_int size2 = RNDUP(size);
    int i;

    if (size2 != size) {
        /* Must zero out extra bytes */
        for (i = size; i < size2; i++)
            objp->data.data_val[i] = 0;
    }

    uio = (xdr_uio *) calloc(1, sizeof(struct xdr_uio) + sizeof(struct xdr_vio));
    uio->uio_release = xdr_READ3res_uio_release;
    uio->uio_count = 1;
    /*todo find what*/
//    uio->uio_vio[0].vio_base = objp->data.data_val;
//    uio->uio_vio[0].vio_head = objp->data.data_val;
//    uio->uio_vio[0].vio_tail = objp->data.data_val + size2;
//    uio->uio_vio[0].vio_wrap = objp->data.data_val + size2;
    uio->uio_vio[0].vio_length = size2;
    uio->uio_vio[0].vio_type = VIO_DATA;

    /* Take over read data buffer */
    objp->data.data_val = NULL;
    objp->data.data_len = 0;

    LOG(MODULE_NAME, D_INFO,
        "Allocated %p, references %d, count %d",
        uio, uio->uio_references, (int) uio->uio_count);

    return uio;
}

inline bool xdr_READ3resok_encode(XDR *xdrs, READ3resok *objp) {
    struct xdr_uio *uio;
    uint32_t size = objp->data.data_len;

    if (!inline_xdr_u_int32_t(xdrs, &size))
        return false;

    uio = xdr_READ3res_uio_setup(objp);

    if (!xdr_putbufs(xdrs, uio, UIO_FLAG_NONE)) {
        uio->uio_release(uio, UIO_FLAG_NONE);
        return false;
    }
    return true;
}

bool xdr_READ3resok(XDR *xdrs, READ3resok *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->file_attributes))
        return (false);
    if (!xdr_count3(xdrs, &objp->count))
        return (false);
    if (!xdr_bool(xdrs, &objp->eof))
        return (false);

    if (xdrs->x_op == XDR_ENCODE)
        return xdr_READ3resok_encode(xdrs, objp);

    if (!xdr_bytes
            (xdrs, (char **) &objp->data.data_val,
             &objp->data.data_len, XDR_BYTES_MAXLEN_IO))
        return (false);
    return (true);
}

bool xdr_READ3resfail(XDR *xdrs, READ3resfail *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->file_attributes))
        return (false);
    return (true);
}

bool xdr_READ3res(XDR *xdrs, READ3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_READ3resok(xdrs, &objp->READ3res_u.resok))
                return (false);
            break;
        default:
            if (!xdr_READ3resfail(xdrs, &objp->READ3res_u.resfail))
                return (false);
            break;
    }
    return (true);
}


#define MODULE_NAME "NFS"