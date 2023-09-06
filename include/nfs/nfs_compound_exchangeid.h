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

#ifndef DNFSD_NFS_COMPOUND_EXCHANGEID_H
#define DNFSD_NFS_COMPOUND_EXCHANGEID_H

#include "nfs/nfs_common_data.h"

struct EXCHANGE_ID4resok {
    clientid4 eir_clientid;
    sequenceid4 eir_sequenceid;
    uint32_t eir_flags;
    state_protect4_r eir_state_protect;
    server_owner4 eir_server_owner;
    struct {
        u_int eir_server_scope_len;
        char *eir_server_scope_val;
    } eir_server_scope;
    struct {
        u_int eir_server_impl_id_len;
        nfs_impl_id4 *eir_server_impl_id_val;
    } eir_server_impl_id;
};
typedef struct EXCHANGE_ID4resok EXCHANGE_ID4resok;

struct EXCHANGE_ID4res {
    nfsstat4 eir_status;
    union {
        EXCHANGE_ID4resok eir_resok4;
    } EXCHANGE_ID4res_u;
};
typedef struct EXCHANGE_ID4res EXCHANGE_ID4res;

#endif //DNFSD_NFS_COMPOUND_EXCHANGEID_H
