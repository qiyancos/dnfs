/*
 * Copyright (c) 2009, Sun Microsystems, Inc.
 * Copyright (c) 2017 Red Hat, Inc. and/or its affiliates.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of Sun Microsystems, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Copyright (c) 1986-1991 by Sun Microsystems Inc.
 */

#include "config.h"
#include <sys/cdefs.h>

/*
 * clnt_simple.c
 * Simplified front end to client rpc.
 */
#include <pthread.h>
#include <reentrant.h>
#include <sys/param.h>
#include <stdio.h>
#include <errno.h>
#include <rpc/rpc.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include <rpc/clnt.h>

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 64
#endif

#ifndef NETIDLEN
#define NETIDLEN 32
#endif

struct rpc_call_private {
	int valid;		/* Is this entry valid ? */
	CLIENT *client;		/* Client handle */
	AUTH *auth;
	pid_t pid;		/* process-id at moment of creation */
	rpcprog_t prognum;	/* Program */
	rpcvers_t versnum;	/* Version */
	char host[MAXHOSTNAMELEN];	/* Servers host */
	char nettype[NETIDLEN];	/* Network type */
};

static void rpc_call_destroy(void *vp)
{
	struct rpc_call_private *rcp = (struct rpc_call_private *)vp;

	if (rcp) {
		if (rcp->client)
			CLNT_DESTROY(rcp->client);
		mem_free(rcp, sizeof(*rcp));
	}
}

static const struct timespec to = { 3, 0 };

/*
 * This is the simplified interface to the client rpc layer.
 * The client handle is not destroyed here and is reused for
 * the future calls to same prog, vers, host and nettype combination.
 *
 * The total time available is 9 seconds.
 */
enum clnt_stat
rpc_call(const char *host,	/* host name */
	 rpcprog_t prognum,	/* program number */
	 rpcvers_t versnum,	/* version number */
	 rpcproc_t procnum,	/* procedure number */
	 xdrproc_t inproc,	/* in XDR procedure */
	 const void *in,
	 xdrproc_t outproc,	/* out XDR procedure */
	 void *out,	/* recv/send data */
	 const char *nettype /* nettype */)
{
	struct rpc_call_private *rcp;
	struct clnt_req *cc;
	enum clnt_stat clnt_stat;
	extern thread_key_t rpc_call_key;
	extern mutex_t tsd_lock;

	if (rpc_call_key == -1) {
		mutex_lock(&tsd_lock);
		if (rpc_call_key == -1)
			thr_keycreate(&rpc_call_key, rpc_call_destroy);
		mutex_unlock(&tsd_lock);
	}
	rcp = (struct rpc_call_private *)thr_getspecific(rpc_call_key);
	if (rcp == NULL) {
		rcp = mem_alloc(sizeof(*rcp));
		thr_setspecific(rpc_call_key, (void *)rcp);
		rcp->valid = 0;
		rcp->client = NULL;
	}
	if ((nettype == NULL) || (nettype[0] == 0))
		nettype = "netpath";
	if (!(rcp->valid && rcp->pid == getpid() && (rcp->prognum == prognum)
	      && (rcp->versnum == versnum) && (!strcmp(rcp->host, host))
	      && (!strcmp(rcp->nettype, nettype)))) {
		int fd;

		rcp->valid = 0;
		if (rcp->client)
			CLNT_DESTROY(rcp->client);
		/*
		 * Using the first successful transport for that type
		 */
		rcp->client = clnt_ncreate(host, prognum, versnum, nettype);
		rcp->pid = getpid();
		clnt_stat = rcp->client->cl_error.re_status;
		if (clnt_stat) {
			CLNT_DESTROY(rcp->client);
			rcp->client = NULL;
			return (clnt_stat);
		}

		rcp->auth = authnone_ncreate();	/* idempotent */

		if (CLNT_CONTROL(rcp->client, CLGET_FD, (char *)(void *)&fd))
			fcntl(fd, F_SETFD, 1);	/* make it "close on exec" */
		rcp->prognum = prognum;
		rcp->versnum = versnum;
		if ((strlen(host) < (size_t) MAXHOSTNAMELEN)
		    && (strlen(nettype) < (size_t) NETIDLEN)) {
			(void)strcpy(rcp->host, host);
			(void)strcpy(rcp->nettype, nettype);
			rcp->valid = 1;
		} else {
			rcp->valid = 0;
		}
	}			/* else reuse old client */

	cc = mem_alloc(sizeof(*cc));
	/* LINTED const castaway */
	clnt_req_fill(cc, rcp->client, rcp->auth, procnum,
		      inproc, (void *)in, outproc, out);
	clnt_stat = clnt_req_setup(cc, to);
	if (clnt_stat == RPC_SUCCESS) {
		clnt_stat = CLNT_CALL_WAIT(cc);
	}
	clnt_req_release(cc);
	/*
	 * if call failed, empty cache
	 */
	if (clnt_stat != RPC_SUCCESS)
		rcp->valid = 0;
	return (clnt_stat);
}
