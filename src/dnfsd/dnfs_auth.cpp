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

#include "dnfsd/dnfs_auth.h"

/*
 * The call rpc message, msg has been obtained from the wire.  The msg contains
 * the raw form of credentials and verifiers.  authenticate returns AUTH_OK
 * if the msg is successfully authenticated.  If AUTH_OK then the routine also
 * does the following things:
 * set req->rq_msg.RPCM_ack.ar_verf to the appropriate response verifier;
 * set req->rq_msg.rq_cred_body to the "cooked" form of the credentials.
 *
 * NB: ar_verf must be pre-allocated, its length is set appropriately.
 *
 * The caller still owns and is responsible for msg->cb_cred and
 * msg->cb_verf.  The authentication system retains ownership of
 * rq_cred_body, the cooked credentials.
 *
 * There is an assumption that any flavour less than AUTH_NULL is invalid.
 */
enum auth_stat svc_auth_authenticate(struct svc_req *req, bool *no_dispatch) {
    struct authsvc *asp;
    enum auth_stat rslt;
    int cred_flavor;

    /* VARIABLES PROTECTED BY authsvc_lock: asp, Auths */
    req->rq_msg.RPCM_ack.ar_verf = _null_auth;
    cred_flavor = req->rq_msg.cb_cred.oa_flavor;
    req->rq_msg.RPCM_ack.ar_verf.oa_flavor = cred_flavor;
    switch (cred_flavor) {
        case AUTH_NONE:
            rslt = _svcauth_none(req);
            return (rslt);
        case AUTH_SYS:
            rslt = _svcauth_unix(req);
            return (rslt);
        case AUTH_SHORT:
            rslt = _svcauth_short(req);
            return (rslt);
        default:
            break;
    }

    /* flavor doesn't match any of the builtin types, so try new ones */
    for (asp = Auths; asp; asp = asp->next) {
        if (asp->flavor == cred_flavor) {
            enum auth_stat as;
            as = (*asp->handler) (req);
            return (as);
        }
    }

    return (AUTH_REJECTEDCRED);
}

/**
 * @brief Converts an auth_stat enum to a string
 *
 * @param[in] why The stat to convert
 *
 * @return String describing the status
 */
const char *auth_stat2str(enum auth_stat why) {
    switch (why) {
        case AUTH_OK:
            return "AUTH_OK";

        case AUTH_BADCRED:
            return "AUTH_BADCRED";

        case AUTH_REJECTEDCRED:
            return "AUTH_REJECTEDCRED";

        case AUTH_BADVERF:
            return "AUTH_BADVERF";

        case AUTH_REJECTEDVERF:
            return "AUTH_REJECTEDVERF";

        case AUTH_TOOWEAK:
            return "AUTH_TOOWEAK";

        case AUTH_INVALIDRESP:
            return "AUTH_INVALIDRESP";

        case AUTH_FAILED:
            return "AUTH_FAILED";

        case RPCSEC_GSS_CREDPROBLEM:
            return "RPCSEC_GSS_CREDPROBLEM";

        case RPCSEC_GSS_CTXPROBLEM:
            return "RPCSEC_GSS_CTXPROBLEM";
    }

    return "UNKNOWN AUTH";
}