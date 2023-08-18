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

#include "rpc/svc.h"
#include "dnfsd/dnfs_meta_data.h"
#include "dnfsd/dnfs_rpc_func.h"

/**
 * @brief Allocate a new request
 *
 * @param[in] xprt Transport to use
 * @param[in] xdrs XDR to use
 *
 * @return New svc request
 */
struct svc_req *alloc_nfs_request(SVCXPRT *xprt, XDR *xdrs)
{
    nfs_request_t *reqdata = reinterpret_cast<nfs_request_t *>(
            calloc(1, sizeof(nfs_request_t)));

    logger.log_if()

    if (!xprt) {
        LogFatal(COMPONENT_DISPATCH,
                 "missing xprt!");
    }

    if (!xdrs) {
        LogFatal(COMPONENT_DISPATCH,
                 "missing xdrs!");
    }

    LogDebug(COMPONENT_DISPATCH,
             "%p fd %d context %p",
             xprt, xprt->xp_fd, xdrs);

    (void) atomic_inc_uint64_t(&nfs_health_.enqueued_reqs);

#ifdef USE_MONITORING
    monitoring_rpc_received();
	monitoring_rpcs_in_flight(
		nfs_health_.enqueued_reqs - nfs_health_.dequeued_reqs);
#endif /* USE_MONITORING*/

    /* set up req */
    SVC_REF(xprt, SVC_REF_FLAG_NONE);
    reqdata->svc.rq_xprt = xprt;
    reqdata->svc.rq_xdrs = xdrs;
    reqdata->svc.rq_refcnt = 1;

    TAILQ_INIT_ENTRY(reqdata, dupes);

    return &reqdata->svc;
}