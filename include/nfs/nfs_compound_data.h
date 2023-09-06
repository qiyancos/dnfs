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

#ifndef DNFSD_NFS_COMPOUND_DATA_H
#define DNFSD_NFS_COMPOUND_DATA_H

#include "nfs/nfs_common_data.h"
#include "nfs/nfs_compound_exchangeid.h"

enum nfs_opnum4 {
    NFS4_OP_ACCESS = 3,
    NFS4_OP_CLOSE = 4,
    NFS4_OP_COMMIT = 5,
    NFS4_OP_CREATE = 6,
    NFS4_OP_DELEGPURGE = 7,
    NFS4_OP_DELEGRETURN = 8,
    NFS4_OP_GETATTR = 9,
    NFS4_OP_GETFH = 10,
    NFS4_OP_LINK = 11,
    NFS4_OP_LOCK = 12,
    NFS4_OP_LOCKT = 13,
    NFS4_OP_LOCKU = 14,
    NFS4_OP_LOOKUP = 15,
    NFS4_OP_LOOKUPP = 16,
    NFS4_OP_NVERIFY = 17,
    NFS4_OP_OPEN = 18,
    NFS4_OP_OPENATTR = 19,
    NFS4_OP_OPEN_CONFIRM = 20,
    NFS4_OP_OPEN_DOWNGRADE = 21,
    NFS4_OP_PUTFH = 22,
    NFS4_OP_PUTPUBFH = 23,
    NFS4_OP_PUTROOTFH = 24,
    NFS4_OP_READ = 25,
    NFS4_OP_READDIR = 26,
    NFS4_OP_READLINK = 27,
    NFS4_OP_REMOVE = 28,
    NFS4_OP_RENAME = 29,
    NFS4_OP_RENEW = 30,
    NFS4_OP_RESTOREFH = 31,
    NFS4_OP_SAVEFH = 32,
    NFS4_OP_SECINFO = 33,
    NFS4_OP_SETATTR = 34,
    NFS4_OP_SETCLIENTID = 35,
    NFS4_OP_SETCLIENTID_CONFIRM = 36,
    NFS4_OP_VERIFY = 37,
    NFS4_OP_WRITE = 38,
    NFS4_OP_RELEASE_LOCKOWNER = 39,

    /* NFSv4.1 */
    NFS4_OP_BACKCHANNEL_CTL = 40,
    NFS4_OP_BIND_CONN_TO_SESSION = 41,
    NFS4_OP_EXCHANGE_ID = 42,
    NFS4_OP_CREATE_SESSION = 43,
    NFS4_OP_DESTROY_SESSION = 44,
    NFS4_OP_FREE_STATEID = 45,
    NFS4_OP_GET_DIR_DELEGATION = 46,
    NFS4_OP_GETDEVICEINFO = 47,
    NFS4_OP_GETDEVICELIST = 48,
    NFS4_OP_LAYOUTCOMMIT = 49,
    NFS4_OP_LAYOUTGET = 50,
    NFS4_OP_LAYOUTRETURN = 51,
    NFS4_OP_SECINFO_NO_NAME = 52,
    NFS4_OP_SEQUENCE = 53,
    NFS4_OP_SET_SSV = 54,
    NFS4_OP_TEST_STATEID = 55,
    NFS4_OP_WANT_DELEGATION = 56,
    NFS4_OP_DESTROY_CLIENTID = 57,
    NFS4_OP_RECLAIM_COMPLETE = 58,

    /* NFSv4.2 */
    NFS4_OP_ALLOCATE = 59,
    NFS4_OP_COPY = 60,
    NFS4_OP_COPY_NOTIFY = 61,
    NFS4_OP_DEALLOCATE = 62,
    NFS4_OP_IO_ADVISE = 63,
    NFS4_OP_LAYOUTERROR = 64,
    NFS4_OP_LAYOUTSTATS = 65,
    NFS4_OP_OFFLOAD_CANCEL = 66,
    NFS4_OP_OFFLOAD_STATUS = 67,
    NFS4_OP_READ_PLUS = 68,
    NFS4_OP_SEEK = 69,
    NFS4_OP_WRITE_SAME = 70,
    NFS4_OP_CLONE = 71,

    /* NFSv4.3 */
    NFS4_OP_GETXATTR = 72,
    NFS4_OP_SETXATTR = 73,
    NFS4_OP_LISTXATTR = 74,
    NFS4_OP_REMOVEXATTR = 75,

    NFS4_OP_LAST_ONE = 76,

    NFS4_OP_ILLEGAL = 10044,
};
typedef enum nfs_opnum4 nfs_opnum4;

struct nfs_argop4 {
    nfs_opnum4 argop;
    union {
//        ACCESS4args opaccess;
//        CLOSE4args opclose;
//        COMMIT4args opcommit;
//        CREATE4args opcreate;
//        DELEGPURGE4args opdelegpurge;
//        DELEGRETURN4args opdelegreturn;
//        GETATTR4args opgetattr;
//        LINK4args oplink;
//        LOCK4args oplock;
//        LOCKT4args oplockt;
//        LOCKU4args oplocku;
//        LOOKUP4args oplookup;
//        NVERIFY4args opnverify;
//        OPEN4args opopen;
//        OPENATTR4args opopenattr;
//        OPEN_CONFIRM4args opopen_confirm;
//        OPEN_DOWNGRADE4args opopen_downgrade;
//        PUTFH4args opputfh;
//        READ4args opread;
//        READDIR4args opreaddir;
//        REMOVE4args opremove;
//        RENAME4args oprename;
//        RENEW4args oprenew;
//        SECINFO4args opsecinfo;
//        SETATTR4args opsetattr;
//        SETCLIENTID4args opsetclientid;
//        SETCLIENTID_CONFIRM4args opsetclientid_confirm;
//        VERIFY4args opverify;
//        WRITE4args opwrite;
//        RELEASE_LOCKOWNER4args oprelease_lockowner;
//        BACKCHANNEL_CTL4args opbackchannel_ctl;
//        BIND_CONN_TO_SESSION4args opbind_conn_to_session;
        EXCHANGE_ID4args opexchange_id;
//        CREATE_SESSION4args opcreate_session;
//        DESTROY_SESSION4args opdestroy_session;
//        FREE_STATEID4args opfree_stateid;
//        GET_DIR_DELEGATION4args opget_dir_delegation;
//        GETDEVICEINFO4args opgetdeviceinfo;
//        GETDEVICELIST4args opgetdevicelist;
//        LAYOUTCOMMIT4args oplayoutcommit;
//        LAYOUTGET4args oplayoutget;
//        LAYOUTRETURN4args oplayoutreturn;
//        SECINFO_NO_NAME4args opsecinfo_no_name;
//        SEQUENCE4args opsequence;
//        SET_SSV4args opset_ssv;
//        TEST_STATEID4args optest_stateid;
//        WANT_DELEGATION4args opwant_delegation;
//        DESTROY_CLIENTID4args opdestroy_clientid;
//        RECLAIM_COMPLETE4args opreclaim_complete;
//
//        /* NFSv4.2 */
//        COPY_NOTIFY4args opoffload_notify;
//        OFFLOAD_REVOKE4args opcopy_revoke;
//        COPY4args opcopy;
//        OFFLOAD_ABORT4args opoffload_abort;
//        OFFLOAD_STATUS4args opoffload_status;
//        WRITE_SAME4args opwrite_same;
//        ALLOCATE4args opallocate;
//        DEALLOCATE4args opdeallocate;
//        READ_PLUS4args opread_plus;
//        SEEK4args opseek;
//        IO_ADVISE4args opio_advise;
//        LAYOUTERROR4args oplayouterror;
//        LAYOUTSTATS4args oplayoutstats;
//
//        /* NFSv4.3 */
//        GETXATTR4args opgetxattr;
//        SETXATTR4args opsetxattr;
//        LISTXATTR4args oplistxattr;
//        REMOVEXATTR4args opremovexattr;

    } nfs_argop4_u;
};
typedef struct nfs_argop4 nfs_argop4;

struct COMPOUND4args {
    utf8str_cs tag;
    uint32_t minorversion;
    struct {
        u_int argarray_len;
        nfs_argop4 *argarray_val;
    } argarray;
};
typedef struct COMPOUND4args COMPOUND4args;

struct nfs_resop4 {
    nfs_opnum4 resop;
    union {
//        ACCESS4res opaccess;
//        CLOSE4res opclose;
//        COMMIT4res opcommit;
//        CREATE4res opcreate;
//        DELEGPURGE4res opdelegpurge;
//        DELEGRETURN4res opdelegreturn;
//        GETATTR4res opgetattr;
//        GETFH4res opgetfh;
//        LINK4res oplink;
//        LOCK4res oplock;
//        LOCKT4res oplockt;
//        LOCKU4res oplocku;
//        LOOKUP4res oplookup;
//        LOOKUPP4res oplookupp;
//        NVERIFY4res opnverify;
//        OPEN4res opopen;
//        OPENATTR4res opopenattr;
//        OPEN_CONFIRM4res opopen_confirm;
//        OPEN_DOWNGRADE4res opopen_downgrade;
//        PUTFH4res opputfh;
//        PUTPUBFH4res opputpubfh;
//        PUTROOTFH4res opputrootfh;
//        READ4res opread;
//        READDIR4res opreaddir;
//        READLINK4res opreadlink;
//        REMOVE4res opremove;
//        RENAME4res oprename;
//        RENEW4res oprenew;
//        RESTOREFH4res oprestorefh;
//        SAVEFH4res opsavefh;
//        SECINFO4res opsecinfo;
//        SETATTR4res opsetattr;
//        SETCLIENTID4res opsetclientid;
//        SETCLIENTID_CONFIRM4res opsetclientid_confirm;
//        VERIFY4res opverify;
//        WRITE4res opwrite;
//        RELEASE_LOCKOWNER4res oprelease_lockowner;
//        BACKCHANNEL_CTL4res opbackchannel_ctl;
//        BIND_CONN_TO_SESSION4res opbind_conn_to_session;
        EXCHANGE_ID4res opexchange_id;
//        CREATE_SESSION4res opcreate_session;
//        DESTROY_SESSION4res opdestroy_session;
//        FREE_STATEID4res opfree_stateid;
//        GET_DIR_DELEGATION4res opget_dir_delegation;
//        GETDEVICEINFO4res opgetdeviceinfo;
//        GETDEVICELIST4res opgetdevicelist;
//        LAYOUTCOMMIT4res oplayoutcommit;
//        LAYOUTGET4res oplayoutget;
//        LAYOUTRETURN4res oplayoutreturn;
//        SECINFO_NO_NAME4res opsecinfo_no_name;
//        SEQUENCE4res opsequence;
//        SET_SSV4res opset_ssv;
//        TEST_STATEID4res optest_stateid;
//        WANT_DELEGATION4res opwant_delegation;
//        DESTROY_CLIENTID4res opdestroy_clientid;
//        RECLAIM_COMPLETE4res opreclaim_complete;
//
//        /* NFSv4.2 */
//        COPY_NOTIFY4res opoffload_notify;
//        OFFLOAD_REVOKE4res opcopy_revoke;
//        COPY4res opcopy;
//        OFFLOAD_ABORT4res opoffload_abort;
//        OFFLOAD_STATUS4res opoffload_status;
//        WRITE_SAME4res opwrite_same;
//        ALLOCATE4res opallocate;
//        DEALLOCATE4res opdeallocate;
//        READ_PLUS4res opread_plus;
//        SEEK4res opseek;
//        IO_ADVISE4res opio_advise;
//        LAYOUTERROR4res oplayouterror;
//        LAYOUTSTATS4res oplayoutstats;
//
//        /* NFSv4.3 */
//        GETXATTR4res opgetxattr;
//        SETXATTR4res opsetxattr;
//        LISTXATTR4res oplistxattr;
//        REMOVEXATTR4res opremovexattr;
//
//        ILLEGAL4res opillegal;
    } nfs_resop4_u;
};
typedef struct nfs_resop4 nfs_resop4;

struct COMPOUND4res {
    nfsstat4 status;
    utf8str_cs tag;
    struct {
        u_int resarray_len;
        nfs_resop4 *resarray_val;
    } resarray;
};
typedef struct COMPOUND4res COMPOUND4res;

typedef struct compound_data compound_data_t;

typedef enum nfs_req_result (*nfs4_function_t)(struct nfs_argop4 *,
                                               compound_data_t *,
                                               struct nfs_resop4 *);

/**
 * #brief Structure to map out how each compound op is managed.
 *
 */
struct nfs4_op_desc {
    /** Operation name */
    char *name;
    /** Function to process the operation */
    nfs4_function_t funct;
    /** Function to resume a suspended operation */
    nfs4_function_t resume;

    /** Function to free the results of the operation.
     *
     * Note this function is called whether the operation succeeds or
     * fails. It may be called as a result of higher level operation
     * completion (depending on DRC handling) or it may be called as part
     * of NFS v4.1 slot cache management.
     *
     * Note that entries placed into the NFS v4.1 slot cache are marked so
     * the higher level operation completion will not release them. A deep
     * copy is made when the slot cache is replayed. If sa_cachethis
     * indicates a response will not be cached, the higher level operation
     * completion will call the free_res, HOWEVER, a shallow copy of the
     * SEQUENCE op and first operation responses are made. If the first
     * operation resulted in an error (other than NFS4_DENIED for LOCK and
     * LOCKT) the shallow copy preserves that error rather than replacing
     * it with NFS4ERR_RETRY_UNCACHED_REP. For this reason for any response
     * that includes dynamically allocated data on NFS4_OK MUST check the
     * response status before freeing any memory since the shallow copy will
     * mean the cached NFS4ERR_RETRY_UNCACHED_REP response will have copied
     * those pointers. It should only free data if the status is NFS4_OK
     * (or NFS4ERR_DENIED in the case of LOCK and LOCKT). Note that
     * SETCLIENTID also has dunamic data on a non-NFS4_OK status, and the
     * free_res function for that checks, however, we will never see
     * SETCLIENTID in NFS v4.1+, or if we do, it will get an error.
     *
     * At this time, LOCK and LOCKT are the only NFS v4.1 or v4.2 operations
     * that have dynamic data on a non-NFS4_OK response. Should any others
     * be added, checks for that MUST be added to the shallow copy code
     * below.
     *
     */
    void (*free_res)(nfs_resop4 *);
    /** Default response size */
    uint32_t resp_size;
    /** Export permissions required flags */
    int exp_perm_flags;
};

#endif //DNFSD_NFS_COMPOUND_DATA_H
