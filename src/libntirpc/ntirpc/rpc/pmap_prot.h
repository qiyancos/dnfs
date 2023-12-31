/*
 * pmap_prot.h
 * Protocol for the local binder service, or pmap.
 *
 * Copyright (c) 2010, Oracle America, Inc.
 * Copyright (c) 2013-2018 Red Hat, Inc. and/or its affiliates.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 *     * Neither the name of the "Oracle America, Inc." nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 *   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 *   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RPC_PMAP_PROT_H
#define _RPC_PMAP_PROT_H 1

#if defined(__linux__)
#include <features.h>
#endif
#include <rpc/xdr.h>

__BEGIN_DECLS
/* The following procedures are supported by the protocol:
 *
 * PMAPPROC_NULL() returns ()
 * takes nothing, returns nothing
 *
 * PMAPPROC_SET(struct pmap) returns (bool)
 * TRUE is success, FALSE is failure.  Registers the tuple
 * [prog, vers, prot, port].
 *
 * PMAPPROC_UNSET(struct pmap) returns (bool)
 * TRUE is success, FALSE is failure.  Un-registers pair
 * [prog, vers].  prot and port are ignored.
 *
 * PMAPPROC_GETPORT(struct pmap) returns (long unsigned).
 * 0 is failure.  Otherwise returns the port number where the pair
 * [prog, vers] is registered.  It may lie!
 *
 * PMAPPROC_DUMP() RETURNS (struct pmaplist *)
 *
 * PMAPPROC_CALLIT(unsigned, unsigned, unsigned, string<>) RETURNS
 * (port, string<>); usage: encapsulatedresults =
 * PMAPPROC_CALLIT(prog, vers, proc, encapsulatedargs); Calls the
 * procedure on the local machine.  If it is not registered, this
 * procedure is quite; ie it does not return error information!!!
 * This procedure only is supported on rpc/udp and calls via rpc/udp.
 * This routine only passes null authentication parameters.  This file
 * has no interface to xdr routines for PMAPPROC_CALLIT.
 *
 * The service supports remote procedure calls on udp/ip or tcp/ip socket 111.
 */
#define PMAPPORT	((rpcport_t)111)
#define PMAPPROG	((rpcprog_t)100000)
#define PMAPVERS	((rpcvers_t)2)
#define PMAPVERS_PROTO	((rpcproc_t)2)
#define PMAPVERS_ORIG	((rpcproc_t)1)
#define PMAPPROC_NULL	((rpcproc_t)0)
#define PMAPPROC_SET	((rpcproc_t)1)
#define PMAPPROC_UNSET	((rpcproc_t)2)
#define PMAPPROC_GETPORT ((rpcproc_t)3)
#define PMAPPROC_DUMP	((rpcproc_t)4)
#define PMAPPROC_CALLIT	((rpcproc_t)5)

struct pmap {
	rpcprog_t pm_prog;
	rpcvers_t pm_vers;
	rpcprot_t pm_prot;
	rpcport_t pm_port;
};

bool xdr_pmap(XDR *__xdrs, struct pmap *__regs);

struct pmaplist {
	struct pmap pml_map;
	struct pmaplist *pml_next;
};

bool xdr_pmaplist(XDR *__xdrs, struct pmaplist **__rp);

__END_DECLS
#endif				/* rpc/pmap_prot.h */
