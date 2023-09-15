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

extern "C" {
#include "rpc/xdr_inline.h"
#include "rpc/xdr.h"
}

#include "nfs/nfs_xdr.h"
#include "log/log.h"

#define MODULE_NAME "DNFS"

bool xdr_nfs3_uint32(XDR *xdrs, nfs3_uint32 *objp) {
    if (!xdr_u_int(xdrs, objp))
        return (false);
    return (true);
}

bool xdr_nfs3_uint64(XDR *xdrs, nfs3_uint64 *objp) {
    if (!xdr_u_longlong_t(xdrs, objp))
        return (false);
    return (true);
}

bool xdr_size3(XDR *xdrs, size3 *objp) {
    if (!xdr_nfs3_uint64(xdrs, objp))
        return (false);
    return (true);
}

bool xdr_nfstime3(XDR *xdrs, nfstime3 *objp) {
    if (!xdr_nfs3_uint32(xdrs, &objp->tv_sec))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->tv_nsec))
        return (false);
    return (true);
}

bool xdr_nfsstat3(XDR *xdrs, nfsstat3 *objp) {
    if (!xdr_enum(xdrs, (enum_t *) objp))
        return (false);
    return (true);
}

bool xdr_ftype3(XDR *xdrs, ftype3 *objp) {
    if (!xdr_enum(xdrs, (enum_t *) objp))
        return (false);
    return (true);
}

bool xdr_uid3(XDR *xdrs, uid3 *objp) {
    if (!xdr_nfs3_uint32(xdrs, objp))
        return (false);
    return (true);
}

bool xdr_gid3(XDR *xdrs, gid3 *objp) {
    if (!xdr_nfs3_uint32(xdrs, objp))
        return (false);
    return (true);
}

bool xdr_specdata3(XDR *xdrs, specdata3 *objp) {
    if (!xdr_nfs3_uint32(xdrs, &objp->specdata1))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->specdata2))
        return (false);
    return (true);
}

bool xdr_fileid3(XDR *xdrs, fileid3 *objp) {
    if (!xdr_nfs3_uint64(xdrs, objp))
        return (false);
    return (true);
}

bool xdr_fattr3(XDR *xdrs, fattr3 *objp) {
    ftype3 ft;
    specdata3 rdev;
    uid3 uid;
    gid3 gid;
    nfstime3 atime, mtime, ctime;
    /*todo 抽象层模式*/
//    mode3 mode;

    if (xdrs->x_op == XDR_ENCODE) {
        /* Convert object_file_type_t to ftype3 */
        switch (objp->type) {
            case FIFO_FILE:
                ft = NF3FIFO;
                break;

            case CHARACTER_FILE:
                ft = NF3CHR;
                break;

            case DIRECTORY:
                ft = NF3DIR;
                break;

            case BLOCK_FILE:
                ft = NF3BLK;
                break;

            case REGULAR_FILE:
            case EXTENDED_ATTR:
                ft = NF3REG;
                break;

            case SYMBOLIC_LINK:
                ft = NF3LNK;
                break;

            case SOCKET_FILE:
                ft = NF3SOCK;
                break;

            default:
                LOG(MODULE_NAME, L_ERROR,
                    "xdr_fattr3: Bogus type = %d",
                    objp->type);
        }
        /*todo模式判定，由于原本使用了抽象层，不进行判定*/
//        mode = fsal2unix_mode(objp->mode);
        rdev.specdata1 = objp->rawdev.major;
        rdev.specdata2 = objp->rawdev.minor;
        uid = objp->owner;
        gid = objp->group;
        atime.tv_sec = objp->atime.tv_sec;
        atime.tv_nsec = objp->atime.tv_nsec;
        mtime.tv_sec = objp->mtime.tv_sec;
        mtime.tv_nsec = objp->mtime.tv_nsec;
        ctime.tv_sec = objp->ctime.tv_sec;
        ctime.tv_nsec = objp->ctime.tv_nsec;
    }

    if (!xdr_ftype3(xdrs, &ft))
        return (false);
//    if (!xdr_mode3(xdrs, &mode))
//        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->numlinks))
        return (false);
    if (!xdr_uid3(xdrs, &uid))
        return (false);
    if (!xdr_gid3(xdrs, &gid))
        return (false);
    if (!xdr_size3(xdrs, &objp->filesize))
        return (false);
    if (!xdr_size3(xdrs, &objp->spaceused))
        return (false);
    if (!xdr_specdata3(xdrs, &rdev))
        return (false);
    if (!xdr_nfs3_uint64(xdrs, &objp->fsid3))
        return (false);
    if (!xdr_fileid3(xdrs, &objp->fileid))
        return (false);
    if (!xdr_nfstime3(xdrs, &atime))
        return (false);
    if (!xdr_nfstime3(xdrs, &mtime))
        return (false);
    if (!xdr_nfstime3(xdrs, &ctime))
        return (false);


    if (xdrs->x_op == XDR_DECODE) {
        /* Convert ftype3 to object_file_type_t */
        switch (ft) {
            case NF3FIFO:
                objp->type = FIFO_FILE;
                break;

            case NF3CHR:
                objp->type = CHARACTER_FILE;
                break;

            case NF3DIR:
                objp->type = DIRECTORY;
                break;

            case NF3BLK:
                objp->type = BLOCK_FILE;
                break;

            case NF3REG:
                objp->type = REGULAR_FILE;
                break;

            case NF3LNK:
                objp->type = SYMBOLIC_LINK;
                break;

            case NF3SOCK:
                objp->type = SOCKET_FILE;
                break;

            default:
                LOG(MODULE_NAME, L_ERROR,
                    "xdr_fattr3: Bogus type = %d",
                    ft);
        }

//        objp->mode = unix2fsal_mode(mode);
        objp->rawdev.major = rdev.specdata1;
        objp->rawdev.minor = rdev.specdata2;
        objp->fsid.major = objp->fsid3;
        objp->fsid.minor = 0;
        objp->owner = uid;
        objp->group = gid;
        objp->atime.tv_sec = atime.tv_sec;
        objp->atime.tv_nsec = atime.tv_nsec;
        objp->mtime.tv_sec = mtime.tv_sec;
        objp->mtime.tv_nsec = mtime.tv_nsec;
        objp->ctime.tv_sec = ctime.tv_sec;
        objp->ctime.tv_nsec = ctime.tv_nsec;
    }

    return (true);
}

bool xdr_post_op_attr(XDR *xdrs, post_op_attr *objp) {
    if (!xdr_bool(xdrs, &objp->attributes_follow))
        return (false);
    switch (objp->attributes_follow) {
        case TRUE:
            if (!xdr_fattr3(xdrs, &objp->post_op_attr_u.attributes))
                return (false);
            break;
        case FALSE:
            break;
        default:
            return (false);
    }
    return (true);
}

bool xdr_nfs_fh3(XDR *xdrs, nfs_fh3 *objp) {
    if (!xdr_bytes
            (xdrs, (char **) &objp->data.data_val,
             (u_int *) &objp->data.data_len, 64))
        return (false);

    return (true);
}


bool xdr_FSINFO3args(XDR *xdrs, FSINFO3args *objp) {
    if (!xdr_nfs_fh3(xdrs, &objp->fsroot))
        return (false);
    return (true);
}

bool xdr_FSINFO3resok(XDR *xdrs, FSINFO3resok *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->obj_attributes))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->rtmax))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->rtpref))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->rtmult))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->wtmax))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->wtpref))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->wtmult))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->dtpref))
        return (false);
    if (!xdr_size3(xdrs, &objp->maxfilesize))
        return (false);
    if (!xdr_nfstime3(xdrs, &objp->time_delta))
        return (false);
    if (!xdr_nfs3_uint32(xdrs, &objp->properties))
        return (false);
    return (true);
}

bool xdr_FSINFO3resfail(XDR *xdrs, FSINFO3resfail *objp) {
    if (!xdr_post_op_attr(xdrs, &objp->obj_attributes))
        return (false);
    return (true);
}

bool xdr_FSINFO3res(XDR *xdrs, FSINFO3res *objp) {
    if (!xdr_nfsstat3(xdrs, &objp->status))
        return (false);
    switch (objp->status) {
        case NFS3_OK:
            if (!xdr_FSINFO3resok(xdrs, &objp->FSINFO3res_u.resok))
                return (false);
            break;
        default:
            if (!xdr_FSINFO3resfail(xdrs, &objp->FSINFO3res_u.resfail))
                return (false);
            break;
    }
    return (true);
}