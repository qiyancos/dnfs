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

#ifndef DNFSD_DNFS_RPC_FUNC_H
#define DNFSD_DNFS_RPC_FUNC_H

/* 为一个新的dnfs请求申请空间并进行相关的初始化操作 */
struct svc_req *alloc_dnfs_request(SVCXPRT *xprt, XDR *xdrs);

#endif //DNFSD_DNFS_RPC_FUNC_H
