/*	$NetBSD: pmap_rmt.h,v 1.7 1998/02/11 23:01:23 lukem Exp $	*/

/*
 * Copyright (c) 2009, Sun Microsystems, Inc.
 * Copyright (c) 2013-2018 Red Hat, Inc. and/or its affiliates.
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
 *
 *	from: @(#)pmap_rmt.h 1.2 88/02/08 SMI
 *	from: @(#)pmap_rmt.h	2.1 88/07/29 4.0 RPCSRC
 * $FreeBSD: src/include/rpc/pmap_rmt.h,v 1.12 2002/03/23 17:24:55 imp Exp $
 */

/*
 * Structures and XDR routines for parameters to and replies from
 * the portmapper remote-call-service.
 *
 * Copyright (C) 1986, Sun Microsystems, Inc.
 */

#ifndef _RPC_PMAP_RMT_H
#define _RPC_PMAP_RMT_H
#include <sys/cdefs.h>

struct rmtcallargs {
	xdrproc_t xdr_args;
	void *args_ptr;
	uint32_t arglen;	/* count of args_ptr */
	rpcprog_t prog;
	rpcvers_t vers;
	rpcproc_t proc;
};

struct rmtcallres {
	xdrproc_t xdr_results;
	void *results_ptr;
	uint32_t resultslen;	/* count of results_ptr */
	rpcport_t port;
};

__BEGIN_DECLS
extern bool xdr_rmtcall_args(XDR *, struct rmtcallargs *);
extern bool xdr_rmtcallres(XDR *, struct rmtcallres *);
__END_DECLS
#endif				/* !_RPC_PMAP_RMT_H */
