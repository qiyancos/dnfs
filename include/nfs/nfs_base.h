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

#ifndef NFS_NFS_FUNC_H
#define NFS_NFS_FUNC_H

extern "C" {
#include "rpc/xdr.h"
}

#include "nfs/nfs_compound_data.h"
#include "nfs/nfs_xdr.h"

#define	NFS_PROGRAM	100003

#define	NFS_V4	4

#define NOTHING_SPECIAL 0x0000	/* Nothing to be done for this kind of
				   request */
#define MAKES_WRITE	0x0001	/* The function modifyes the FSAL
				   (not permitted for RO FS) */
#define NEEDS_CRED	0x0002	/* A credential is needed for this
				   operation */
#define CAN_BE_DUP	0x0004	/* Handling of dup request can be done
				   for this request */
#define SUPPORTS_GSS	0x0008	/* Request may be authenticated by
				   RPCSEC_GSS */
#define MAKES_IO	0x0010	/* Request may do I/O
				   (not allowed on MD ONLY exports */

typedef union nfs_arg__ {
    COMPOUND4args arg_compound4;
} nfs_arg_t;

struct COMPOUND4res_extended {
    COMPOUND4res res_compound4;
    int32_t res_refcnt;
};

typedef union nfs_res__ {
    struct COMPOUND4res_extended *res_compound4_extended;
} nfs_res_t;

/* 标准RPC处理函数类别 */
typedef int (*nfs_protocol_function_t) (nfs_arg_t *,
                                        struct svc_req *,
                                        nfs_res_t *);

/* 标准RPC函数后处理释放空间处理类别 */
typedef void (*nfs_protocol_free_t) (nfs_res_t *);

/* RPC函数的单个函数描述符结构体 */
typedef struct nfs_function_desc__ {
    nfs_protocol_function_t service_function;
    nfs_protocol_free_t free_function;
    xdrproc_t xdr_decode_func;
    xdrproc_t xdr_encode_func;
    const char *funcname;
    unsigned int dispatch_behaviour;
} nfs_function_desc_t;

/* 无效操作函数的相应处理类别 */
const nfs_function_desc_t invalid_funcdesc = {
        .service_function = NULL,
        .free_function = NULL,
        .xdr_decode_func = (xdrproc_t) xdr_void,
        .xdr_encode_func = (xdrproc_t) xdr_void,
        .funcname = "invalid_function",
        .dispatch_behaviour = NOTHING_SPECIAL
};

/* NFS NULL Process function*/
int nfs_null([[maybe_unused]] nfs_arg_t *arg,
             [[maybe_unused]] struct svc_req *req,
             [[maybe_unused]] nfs_res_t *res);

/* NFS FREE Process Function */
void nfs_null_free([[maybe_unused]] nfs_res_t *res);

/* NFS COUNPOUND Process Function */
int nfs4_compound(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res);

/* NFS COUNPOUND FREE Process Function */
void nfs4_compound_free(nfs_res_t *res);

/* NFS4，RPC相关函数的处理描述信息，包括函数、xdr函数、函数名等等 */
const nfs_function_desc_t nfs4_func_desc[] = {
    {
        .service_function = nfs_null,
        .free_function = nfs_null_free,
        .xdr_decode_func = (xdrproc_t) xdr_void,
        .xdr_encode_func = (xdrproc_t) xdr_void,
        .funcname = "NFS_NULL",
        .dispatch_behaviour = NOTHING_SPECIAL
    },{
        .service_function = nfs4_compound,
        .free_function = nfs4_compound_free,
        .xdr_decode_func = (xdrproc_t) xdr_COMPOUND4args,
        .xdr_encode_func = (xdrproc_t) xdr_COMPOUND4res_extended,
        .funcname = "NFS4_COMP",
        .dispatch_behaviour = CAN_BE_DUP
    }
};

/* NFS4的基本配置参数 */
enum recovery_backend {
    RECOVERY_BACKEND_FS,
    RECOVERY_BACKEND_FS_NG,
    RECOVERY_BACKEND_RADOS_KV,
    RECOVERY_BACKEND_RADOS_NG,
    RECOVERY_BACKEND_RADOS_CLUSTER,
};

typedef struct nfs_version4_parameter {
    /** Whether to disable the NFSv4 grace period.  Defaults to
        false and settable with Graceless. */
    bool graceless;
    /** The NFSv4 lease lifetime.  Defaults to
        LEASE_LIFETIME_DEFAULT and is settable with
        Lease_Lifetime. */
    uint32_t lease_lifetime;
    /** The NFS grace period.  Defaults to
        GRACE_PERIOD_DEFAULT and is settable with Grace_Period. */
    uint32_t grace_period;
    /** The eir_server_scope for lock recovery. Defaults to NULL
        and is settable with server_scope. */
    char *server_scope;
    /** The eir_server_owner. Defaults to NULL and is settable
        with server_owner. */
    char *server_owner;
    /** Domain to use if we aren't using the nfsidmap.  Defaults
        to DOMAINNAME_DEFAULT and is set with DomainName. */
    char *domainname;
    /** Path to the idmap configuration file.  Defaults to
        IDMAPCONF_DEFAULT, settable with IdMapConf */
    char *idmapconf;
    /** Full path to recovery root directory */
    char *recov_root;
    /** Name of recovery directory */
    char *recov_dir;
    /** Name of recovery old dir (for legacy recovery_fs only */
    char *recov_old_dir;
    /** Whether to use local password (PAM, on Linux) rather than
        nfsidmap.  Defaults to false if nfsidmap support is
        compiled in and true if it isn't.  Settable with
        UseGetpwnam. */
    bool use_getpwnam;
    /** Whether to allow bare numeric IDs in NFSv4 owner and
        group identifiers.  Defaults to true and is settable with
        Allow_Numeric_Owners. */
    bool allow_numeric_owners;
    /** Whether to ONLY use bare numeric IDs in NFSv4 owner and
        group identifiers.  Defaults to false and is settable with
        Only_Numeric_Owners. NB., this is permissible for a server
        implementation (RFC 5661). */
    bool only_numeric_owners;
    /** Whether to allow delegations. Defaults to false and settable
        with Delegations */
    bool allow_delegations;
    /** Delay after which server will retry a recall in case of failures */
    uint32_t deleg_recall_retry_delay;
    /** Whether this a pNFS MDS server. Defaults to false */
    bool pnfs_mds;
    /** Whether this a pNFS DS server. Defaults to false */
    bool pnfs_ds;
    /** Recovery backend */
    enum recovery_backend recovery_backend;
    /** List of supported NFSV4 minor versions */
    unsigned int minor_versions;
    /** Number of allowed slots in the 4.1 slot table */
    uint32_t nb_slots;
    /** whether to skip utf8 validation. defaults to false and settable
         with enforce_utf8_validation. */
    bool enforce_utf8_vld;
    /** Max number of Client IDs allowed on the system */
    uint32_t max_client_ids;
} nfs_version4_parameter_t;

#endif //NFS_NFS_FUNC_H
