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
#include "nfs/nfs_write.h"
#include "nfs/nfs_xdr.h"
#include "nfs/nfs_utils.h"
#include "log/log.h"
#include "dnfsd/dnfs_meta_data.h"

#define MODULE_NAME "NFS"

int nfs3_write(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res){
    int rc = NFS_REQ_OK;

    if (arg->arg_write3.file.data.data_len == 0) {
        rc = NFS_REQ_ERROR;
        LOG(MODULE_NAME, L_ERROR,
            "arg_write get file handle len is 0");
        goto out;
    }


    get_file_handle(arg->arg_write3.file);

    LOG(MODULE_NAME, D_INFO,
        "The value of the arg_write obtained file handle is '%s', and the length is '%d'",
        arg->arg_write3.file.data.data_val,
        arg->arg_write3.file.data.data_len);

    out:

    return rc;
}


void nfs3_write_free(nfs_res_t *res){

}

bool xdr_stable_how(XDR *xdrs, stable_how *objp)
{
    if (!xdr_enum(xdrs, (enum_t *) objp))
        return (false);
    return (true);
}

bool xdr_WRITE3args(XDR *xdrs, WRITE3args *objp)
{
    struct nfs_request_lookahead *lkhd =
            xdrs->x_public ? (struct nfs_request_lookahead *)xdrs->
                    x_public : &dummy_lookahead;

    if (!xdr_nfs_fh3(xdrs, &objp->file))
        return (false);
    if (!xdr_offset3(xdrs, &objp->offset))
        return (false);
    if (!xdr_count3(xdrs, &objp->count))
        return (false);
    if (!xdr_stable_how(xdrs, &objp->stable))
        return (false);
    if (!xdr_bytes
            (xdrs, (char **)&objp->data.data_val,
             &objp->data.data_len, XDR_BYTES_MAXLEN_IO))
        return (false);
    lkhd->flags |= NFS_LOOKAHEAD_WRITE;
    (lkhd->write)++;
    return (true);
}

bool xdr_WRITE3resok(XDR *xdrs, WRITE3resok *objp)
{
    if (!xdr_wcc_data(xdrs, &objp->file_wcc))
        return (false);
    if (!xdr_count3(xdrs, &objp->count))
        return (false);
    if (!xdr_stable_how(xdrs, &objp->committed))
        return (false);
    if (!xdr_writeverf3(xdrs, objp->verf))
        return (false);
    return (true);
}

bool xdr_WRITE3resfail(XDR *xdrs, WRITE3resfail *objp)
{
    if (!xdr_wcc_data(xdrs, &objp->file_wcc))
        return (false);
    return (true);
}

bool xdr_WRITE3res(XDR *xdrs, WRITE3res *objp)
{
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_WRITE3resok(xdrs, &objp->WRITE3res_u.resok))
                return (false);
            break;
        default:
            if (!xdr_WRITE3resfail(xdrs, &objp->WRITE3res_u.resfail))
                return (false);
            break;
    }
    return (true);
}