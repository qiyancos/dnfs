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

#include "dnfsd/dnfs_meta_data.h"

/**
 * @brief Compound data
 *
 * This structure contains the necessary stuff for keeping the state
 * of a V4 compound request.
 */
struct compound_data {
//    nfs_fh4 currentFH;	/*< Current filehandle */
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
//    struct timespec op_start_time;
    nfs_argop4 *argarray;
    nfs_res_t *res;
//    nfs_resop4 *resarray;
    uint32_t argarray_len;
//    nfs_client_cred_t credential;	/*< Raw RPC credentials */
//    nfs_client_id_t *preserved_clientid;	/*< clientid that has lease
//						   reserved, if any */
//    struct nfs41_session_slot__ *slot;	/*< NFv41: pointer to the
//							session's slot */
//    nfsstat4 cached_result_status; /* <NFv41: save the
//	    slot->cached_result->status for reply request */
//    bool sa_cachethis;	/*< True if cachethis was specified in
//				    SEQUENCE op. */
//    nfs_opnum4 opcode;	/*< Current NFS4 OP */
//    uint32_t oppos;		/*< Position of the operation within the
//				    request processed  */
//    const char *opname;	/*< Name of the operation */
//    char *tagname;
//    void *op_data;		/*< operation specific data for resume */
//    nfs41_session_t *session;	/*< Related session
//					   (found by OP_SEQUENCE) */
//    sequenceid4 sequence;	/*< Sequence ID of the current compound
//				   (if applicable) */
//    slotid4 slotid;		/*< Slot ID of the current compound
//				   (if applicable) */
//    uint32_t resp_size;	/*< Running total response size. */
//    uint32_t op_resp_size;	/*< Current op's response size. */
};

typedef struct compound_data compound_data_t;

#endif //DNFSD_NFS_COMPOUND_BASE_H
