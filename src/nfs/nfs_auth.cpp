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

#include "nfs/nfs_auth.h"

int nfs_rpc_req2client_cred(struct svc_req *req, nfs_client_cred_t *pcred) {
    /* Structure for managing basic AUTH_UNIX authentication */
    struct authunix_parms *aup = NULL;

    pcred->length = req->rq_msg.cb_cred.oa_length;
    pcred->flavor = req->rq_msg.cb_cred.oa_flavor;

    switch (req->rq_msg.cb_cred.oa_flavor) {
        case AUTH_NONE:
            /* Do nothing... */
            break;

        case AUTH_UNIX:
            aup = (struct authunix_parms *)req->rq_msg.rq_cred_body;

            pcred->auth_union.auth_unix.aup_uid = aup->aup_uid;
            pcred->auth_union.auth_unix.aup_gid = aup->aup_gid;
            pcred->auth_union.auth_unix.aup_time = aup->aup_time;

            break;

        default:
            /* Unsupported authentication flavour */
            return -1;
    }

    return 1;
}
