
#include "dnfsd/dnfs_worker.h"
#include "dnfsd/dnfs_dispatcher.h"
#include "log/display.h"
#include "log/log.h"

#define container_of(addr, type, member) ({			\
	const typeof(((type *) 0)->member) * __mptr = (addr);	\
	(type *)((char *) __mptr - offsetof(type, member)); })


/**
 * @brief Report Invalid Program number
 *
 * @param[in] reqnfs	NFS request
 *
 */
static enum xprt_stat nfs_rpc_noprog(nfs_request_t *reqdata)
{
    LogFullDebug(COMPONENT_DISPATCH,
                 "Invalid Program number %" PRIu32,
                 reqdata->svc.rq_msg.cb_prog);
    return svcerr_noprog(&reqdata->svc);
}

/**
 * @brief Report Invalid protocol Version
 *
 * @param[in] reqnfs	NFS request
 *
 */
static enum xprt_stat nfs_rpc_novers(nfs_request_t *reqdata,
                                     int lo_vers, int hi_vers)
{
    LogFullDebug(COMPONENT_DISPATCH,
                 "Invalid protocol Version %" PRIu32
                         " for Program number %" PRIu32,
                 reqdata->svc.rq_msg.cb_vers,
                 reqdata->svc.rq_msg.cb_prog);
    return svcerr_progvers(&reqdata->svc, lo_vers, hi_vers);
}

/**
 * @brief Validate rpc calls, extract nfs function descriptor.
 *
 * Validate the rpc call program, version, and procedure within range.
 * Send svcerr_* reply on errors.
 *
 * Choose the function descriptor, either a valid one or the default
 * invalid handler.
 *
 * @param[in,out] req service request
 *
 * @return whether the request is valid.
 */
enum xprt_stat nfs_rpc_valid_NFS(struct svc_req *req)
{
    nfs_request_t *reqdata =
    container_of(req, struct nfs_request, svc);
    int lo_vers;
    int hi_vers;
#ifdef USE_LTTNG
    SVCXPRT *xprt = reqdata->svc.rq_xprt;

	tracepoint(nfs_rpc, valid, __func__, __LINE__, xprt,
		   (unsigned int) req->rq_msg.cb_prog,
		   (unsigned int) req->rq_msg.cb_vers,
		   (unsigned int) reqdata->svc.rq_msg.cb_proc);
#endif

    // reqdata->funcdesc = &invalid_funcdesc;

#ifdef USE_NFSACL3
    if (req->rq_msg.cb_prog == NFS_program[P_NFSACL]) {
		if (req->rq_msg.cb_vers == NFSACL_V3 && CORE_OPTION_NFSV3) {
			if (req->rq_msg.cb_proc <= NFSACLPROC_SETACL) {
				reqdata->funcdesc =
					&nfsacl_func_desc[req->rq_msg.cb_proc];
				return nfs_rpc_process_request(reqdata, false);
			}
		}
	}
#endif /* USE_NFSACL3 */

    if (req->rq_msg.cb_prog != NFS_program[P_NFS]) {
        return nfs_rpc_noprog(reqdata);
    }

#ifdef _USE_NFS3
    if (req->rq_msg.cb_vers == NFS_V3 && NFS_options & CORE_OPTION_NFSV3) {
		if (req->rq_msg.cb_proc <= NFSPROC3_COMMIT) {
			reqdata->funcdesc =
				&nfs3_func_desc[req->rq_msg.cb_proc];
			return nfs_rpc_process_request(reqdata, false);
		}
		return nfs_rpc_noproc(reqdata);
	}
#endif /* _USE_NFS3 */

    /* Unsupported version! Set low and high versions correctly */
    if (NFS_options & CORE_OPTION_NFSV4)
        hi_vers = NFS_V4;
    else
        hi_vers = NFS_V3;
#ifdef _USE_NFS3
    if (NFS_options & CORE_OPTION_NFSV3)
		lo_vers = NFS_V3;
	else
		lo_vers = NFS_V4;
#else
    lo_vers = NFS_V4;
#endif

    return nfs_rpc_novers(reqdata, lo_vers, hi_vers);
}