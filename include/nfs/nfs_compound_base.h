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

#ifndef DNFSD_NFS_COMPOUND_BASE_H
#define DNFSD_NFS_COMPOUND_BASE_H

/* 该文件定义的内容只有compound执行文件才会用到 */

extern "C" {
#include "rpc/auth_unix.h"
}

#include "nfs/nfs_common_data.h"
#include "dnfsd/dnfs_meta_data.h"

/**
 * @brief Default string length of all operations in one compound
 */
#define NFS4_COMPOUND_OPERATIONS_STR_LEN 256

typedef struct nfs_client_cred_gss {
    unsigned int svc;
    unsigned int qop;
#ifdef _HAVE_GSSAPI
    struct svc_rpc_gss_data *gd;
#endif
} nfs_client_cred_gss_t;

typedef struct nfs_client_cred__ {
    unsigned int flavor;
    unsigned int length;
    union {
        struct authunix_parms auth_unix;
        nfs_client_cred_gss_t auth_gss;
    } auth_union;
} nfs_client_cred_t;

struct channel_attrs4 {
    count4 ca_headerpadsize;
    count4 ca_maxrequestsize;
    count4 ca_maxresponsesize;
    count4 ca_maxresponsesize_cached;
    count4 ca_maxoperations;
    count4 ca_maxrequests;
    struct {
        u_int ca_rdma_ird_len;
        uint32_t *ca_rdma_ird_val;
    } ca_rdma_ird;
};
typedef struct channel_attrs4 channel_attrs4;

/**
 * @brief Structure representing an NFSv4.1 session
 */
struct nfs41_session {
//    char session_id[NFS4_SESSIONID_SIZE];	/*< Session ID */
//    struct glist_head session_link;	/*< Link in the list of
//					   sessions for this
//					   clientid */
//
    channel_attrs4 fore_channel_attrs;	/*< Fore-channel attributes */
//
//    channel_attrs4 back_channel_attrs;	/*< Back-channel attributes */
//    struct rpc_call_channel cb_chan;	/*< Back channel */
//    pthread_mutex_t cb_mutex;	/*< Protects the cb slot table,
//					   when searching for a free slot */
//    pthread_cond_t cb_cond;	/*< Condition variable on which we
//				   wait if the slot table is full
//				   and on which we signal when we
//				   free an entry. */
//
//    pthread_rwlock_t conn_lock;
//    int num_conn;
//    sockaddr_t connections[NFS41_MAX_CONNECTIONS];
//
//    nfs_client_id_t *clientid_record;	/*< Client record
//						   corresponding to ID */
//    clientid4 clientid;	/*< Clientid owning this session */
//    uint32_t cb_program;	/*< Callback program ID */
//    uint32_t flags;		/*< Flags pertaining to this session */
//    int32_t refcount;
//    uint32_t nb_slots;	/**< Number of slots in this session */
//    nfs41_session_slot_t *fc_slots;	/**< Forechannel slot table*/
//    nfs41_cb_session_slot_t *bc_slots;	/**< Backchannel slot table */
};

/**
 * @brief Compound data
 *
 * This structure contains the necessary stuff for keeping the state
 * of a V4 compound request.
 */
struct compound_data {
    nfs_fh4 currentFH;	/*< Current filehandle */
//    nfs_fh4 savedFH;	/*< Saved filehandle */
//    stateid4 current_stateid;	/*< Current stateid */
//    bool current_stateid_valid;	/*< Current stateid is valid */
//    stateid4 saved_stateid;	/*< Saved stateid */
//    bool saved_stateid_valid;	/*< Saved stateid is valid */
    unsigned int minorversion;	/*< NFSv4 minor version */
//    struct fsal_obj_handle *current_obj;	/*< Current object handle */
//    struct fsal_obj_handle *saved_obj;	/*< saved object handle */
//    struct fsal_ds_handle *current_ds;	/*< current ds handle */
//    struct fsal_ds_handle *saved_ds;	/*< Saved DS handle */
//    object_file_type_t current_filetype;    /*< File type of current obj */
//    object_file_type_t saved_filetype;	/*< File type of saved entry */
//    struct gsh_export *saved_export; /*< Export entry related to the
//					     savedFH */
//    struct fsal_pnfs_ds *saved_pnfs_ds; /*< DS related to the savedFH */
//    struct export_perms saved_export_perms; /*< Permissions for export for
//					       savedFH */
    struct svc_req *req;	/*< RPC Request related to the compound */
    struct timespec op_start_time;
    nfs_argop4 *argarray;
    nfs_res_t *res;
    nfs_resop4 *resarray;
    uint32_t argarray_len;
    nfs_client_cred_t credential;	/*< Raw RPC credentials */
//    nfs_client_id_t *preserved_clientid;	/*< clientid that has lease
//						   reserved, if any */
//    struct nfs41_session_slot__ *slot;	/*< NFv41: pointer to the
//							session's slot */
//    nfsstat4 cached_result_status; /* <NFv41: save the
//	    slot->cached_result->status for reply request */
//    bool sa_cachethis;	/*< True if cachethis was specified in
//				    SEQUENCE op. */
    nfs_opnum4 opcode;	/*< Current NFS4 OP */
    uint32_t oppos;		/*< Position of the operation within the
				    request processed  */
    const char *opname;	/*< Name of the operation */
    char *tagname;
//    void *op_data;		/*< operation specific data for resume */
    nfs41_session *session;	/*< Related session
					   (found by OP_SEQUENCE) */
//    sequenceid4 sequence;	/*< Sequence ID of the current compound
//				   (if applicable) */
//    slotid4 slotid;		/*< Slot ID of the current compound
//				   (if applicable) */
    uint32_t resp_size;	/*< Running total response size. */
    uint32_t op_resp_size;	/*< Current op's response size. */
};

typedef struct compound_data compound_data_t;

/** Define the last valid NFS v4 op for each minor version.
 *
 */
nfs_opnum4 LastOpcode[] = {
    NFS4_OP_RELEASE_LOCKOWNER,
    NFS4_OP_RECLAIM_COMPLETE,
    NFS4_OP_REMOVEXATTR
};

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

extern const struct nfs4_op_desc optabv4[];

#define NFS4_SESSIONID_SIZE 16

#endif //DNFSD_NFS_COMPOUND_BASE_H
