//
// Created by iecas on 2023/8/4.
//

#ifndef DNFS_DNFSCTL_H
#define DNFS_DNFSCTL_H

#include <linux/slab.h>
#include <linux/namei.h>
#include <linux/ctype.h>
#include <linux/fs_context.h>

#include <linux/sunrpc/svcsock.h>
#include <linux/lockd/lockd.h>
#include <linux/sunrpc/addr.h>
#include <linux/sunrpc/gss_api.h>
#include <linux/sunrpc/gss_krb5_enctypes.h>
#include <linux/sunrpc/rpc_pipe_fs.h>
#include <linux/module.h>
#include <linux/fsnotify.h>
#include <linux/fs.h>

#include <sys/types.h>

#include "dnfsd/idmap.h"
#include "dnfsd/dnfsd.h"
#include "dnfsd/cache.h"
#include "dnfsd/state.h"
#include "dnfsd/netns.h"
#include "dnfsd/pnfs.h"

static ssize_t write_versions(char *buf, size_t size);

#define CONFIG_NFSD_V4

#endif //DNFS_DNFSCTL_H
