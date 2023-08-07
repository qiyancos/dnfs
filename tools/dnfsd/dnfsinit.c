/*
 * utils/nfsd/nfssvc.c
 *
 * Run an DNFS daemon.
 *
 * Copyright (C) 1995, 1996 Olaf Kirch <okir@monad.swb.de>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "utils/xlog.h"
#include "dnfsd/dnfs.h"
#include "dnfsd/dnfsd.h"
#include "dnfsd/nfslib.h"
#include "dnfsd/dnfsinit.h"
#include "dnfsd/dnfsctl.h"

#ifndef DNFSD_FS_DIR
#define DNFSD_FS_DIR	  "/proc/fs/dnfsd"
#endif

#define DNFSD_PORTS_FILE   DNFSD_FS_DIR "/portlist"
#define DNFSD_VERS_FILE    DNFSD_FS_DIR "/versions"
#define DNFSD_THREAD_FILE  DNFSD_FS_DIR "/threads"

/*----------------------------------------------------------------------------*/
/*
 * payload - write methods
 */

static inline struct net *netns(struct file *file)
{
    return file_inode(file)->i_sb->s_fs_info;
}

/*
 * 创建DNFS的运行时目录
 */
int
dnfssvc_create_status_dir(char *progname)
{
	int err;
	struct stat statbuf;

    if (stat(DNFSD_FS_DIR, &statbuf) == -1) {
        if (mkdir(DNFSD_FS_DIR, 0500) != SUCCESS) {
            xlog(L_ERROR, "Unable to create directory %s: errno %d (%m)",
                 DNFSD_THREAD_FILE, errno);
            return errno;
        }
	} else {
        xlog(D_GENERAL, "Directory already exists: %s", DNFSD_FS_DIR);
    }
    return SUCCESS;
}

/*
 * 判断当前DNFSD服务是否处于启用状态
 */
int
dnfssvc_inuse(void)
{
	int fd, n;

	fd = open(DNFSD_PORTS_FILE, O_RDONLY);
    if (fd < 0) {
        return 0;
    }

	/* problem opening file, assume that nothing is configured */
	if (fd < 0)
        xlog(D_GENERAL, "dnfsd is currently down");
		return 0;

	n = read(fd, buf, sizeof(buf));
	close(fd);

	xlog(D_GENERAL, "dnfsd is currently %s", (n > 0) ? "up" : "down");

	return (n > 0);
}

/*
 * 设置DNFSD的服务支持版本信息
 */
int
dnfssvc_setvers(unsigned int ctlbits, unsigned int minorvers)
{
    mutex_lock(&nfsd_mutex);

    struct nfsd_net *nn = net_generic(netns(file), nfsd_net_id);
    if (nn->nfsd_serv) {
        /* Cannot change versions without updating
         * nn->nfsd_serv->sv_xdrsize, and reallocing
         * rq_argp and rq_resp
         */
        xlog(L_ERROR, "Cannot change versions while nfsd is busy");
        return -EBUSY;
    }

    int fd, off = 0;
    char buf[128];
	fd = open(DNFSD_VERS_FILE, O_WRONLY);
	if (fd < 0) {
        xlog(L_ERROR, "Failed to open version file: %m");
        return -ENOENT;
    }

    // 处理大版本，目前主要是2和3两个版本
    enum vers_op cmd;
    unsigned minor = 0;
    int n = 0;
	for (n = DNFSD_MINVERS; n <= DNFSD_MAXVERS; n++) {
        if (n == 4 && minorvers == 0) {
            cmd = DNFSCTL_VERISSET(ctlbits, n) ? NFSD_SET : NFSD_CLEAR;
            if ((cmd == NFSD_SET) != nfsd_vers(nn, n, NFSD_TEST)) {
                /*
                 * Either we have +4 and no minors are enabled,
                 * or we have -4 and at least one minor is enabled.
                 * In either case, propagate 'cmd' to all minors.
                 */
                while (nfsd_minorversion(nn, minor, cmd) >= 0)
                    minor++;
            }
            off += snprintf(buf+off, sizeof(buf) - off, "%c%d ",
                            DNFSCTL_VERISSET(ctlbits, n) ? '+' : '-', n);
        } else if (n == 4) {
            // 如果出现小版本号，那么一定是nfsv4协议版本
            for (minor = DNFS4_MINMINOR; minor <= DNFS4_MAXMINOR; minor++) {
                if (DNFSCTL_VERISSET(minorvers, minor)) {
                    cmd = DNFSCTL_VERISSET(minorvers, minor) ? NFSD_SET : NFSD_CLEAR;
                    if (nfsd_minorversion(nn, minor, cmd) < 0)
                        return -EINVAL;
                }
            }
            off += snprintf(buf+off, sizeof(buf) - off, "%c4.%d ",
                            DNFSCTL_VERISSET(minorvers, minor) ? '+' : '-', minor);
        } else {
            cmd = DNFSCTL_VERISSET(ctlbits, n) ? NFSD_SET : NFSD_CLEAR;
            nfsd_vers(nn, n, cmd);
            off += snprintf(buf+off, sizeof(buf) - off, "%c%d ",
                            DNFSCTL_VERISSET(ctlbits, n) ? '+' : '-', n);
        }
	}
    nfsd_reset_versions(nn);

	xlog(D_GENERAL, "Writing version string to kernel: %s", buf);
	snprintf(buf+off, sizeof(buf) - off, "\n");
    if (write(fd, buf, strlen(buf)) != (ssize_t)strlen(buf)) {
        xlog(L_ERROR, "Failed to write version info to %s: %m", DNFSD_VERS_FILE);
    }
    close(fd);

    mutex_unlock(&nfsd_mutex);
    return SUCCESS;
}

/*
 * 设置服务的grace或者lease时间
 */
int
dnfssvc_set_time(const char *type, const int seconds)
{
    mutex_lock(&nfsd_mutex);

    char pathbuf[40];
    snprintf(pathbuf, sizeof(pathbuf), DNFSD_FS_DIR "/nfsv4%stime", type);
    int fd = open(pathbuf, O_WRONLY);
    if (fd < 0) {
        xlog(L_ERROR, "Failed to open nfs %stime file: %m", type);
        return errno;
    }

    struct nfsd_net *nn = net_generic(netns(file), nfsd_net_id);
    if (nn->nfsd_serv) {
        /* Cannot change versions without updating
         * nn->nfsd_serv->sv_xdrsize, and reallocing
         * rq_argp and rq_resp
         */
        xlog(L_ERROR, "Cannot change versions while nfsd is busy");
        return -EBUSY;
    }

    /*
     * Some sanity checking.  We don't have a reason for
     * these particular numbers, but problems with the
     * extremes are:
     *	- Too short: the briefest network outage may
     *	  cause clients to lose all their locks.  Also,
     *	  the frequent polling may be wasteful.
     *	- Too long: do you really want reboot recovery
     *	  to take more than an hour?  Or to make other
     *	  clients wait an hour before being able to
     *	  revoke a dead client's locks?
     */
    if (seconds < 10 || seconds > 3600)
        return -EINVAL;
    if (strcmp(type, "grace") == 0) {
        nn->nfsd4_grace = seconds;
    } else {
        nn->nfsd4_lease = seconds;
    }

    char nbuf[10];
    snprintf(nbuf, sizeof(nbuf), "%d", seconds);
    if (write(fd, nbuf, strlen(nbuf)) != (ssize_t)strlen(nbuf)) {
        xlog(L_ERROR, "Failed to write nfsv4%stime to %s: %m", type, pathbuf);
    }
    close(fd);

//    if (strcmp(type, "grace") == 0) {
//        /* set same value for lockd */
//        fd = open("/proc/sys/fs/nfs/nlm_grace_period", O_WRONLY);
//        if (fd >= 0) {
//            write(fd, nbuf, strlen(nbuf));
//            close(fd);
//        }
//    }

    mutex_unlock(&nfsd_mutex);
    return SUCCESS;
}

/*
 * 根据指定的socket文件描述符创建相关的服务并绑定到socket
 */
int
dnfssvc_write_ports(const int sockfd) {
    mutex_lock(&nfsd_mutex);

    char *mesg = buf;
    int err = SUCCESS;
    struct net *net = netns(file);
    struct nfsd_net *nn = net_generic(net, nfsd_net_id);

    if (svc_alien_sock(net, sockfd)) {
        xlog(L_ERROR, "%s: socket net is different to NFSd's one\n", __func__);
        return -EINVAL;
    }

    err = nfsd_create_serv(net);
    if (err != 0)
        return err;

    char buf[128];
    err = svc_addsock(nn->nfsd_serv, sockfd, buf,
                      sysconf(_SC_PAGESIZE) - sizeof(struct simple_transaction_argresp),
                              file->f_cred);
    if (err < 0) {
        nfsd_destroy(net);
        return err;
    }

    /* Decrease the count, but don't shut down the service */
    nn->nfsd_serv->sv_nrthreads--;

    mutex_unlock(&nfsd_mutex);
    return errno = err;
}

/*
 * 为即将启动的nfs服务初始化socket接口
 */
int
dnfssvc_set_sockets(const unsigned int protobits,
                   const char *node, const char *port)
{
    struct addrinfo hints = { .ai_flags = AI_PASSIVE };

#ifdef IPV6_SUPPORTED
    hints.ai_family = AF_UNSPEC;
#else  /* IPV6_SUPPORTED */
    hints.ai_family = AF_INET;
#endif /* IPV6_SUPPORTED */

    if (!DNFSCTL_ANYPROTO(protobits))
        return EPROTOTYPE;
    else if (!DNFSCTL_UDPISSET(protobits))
        hints.ai_protocol = IPPROTO_TCP;
    else if (!DNFSCTL_TCPISSET(protobits))
        hints.ai_protocol = IPPROTO_UDP;

    int fd, on = 1, fac = L_ERROR;
    int sockfd = -1, rc = 0;
    struct addrinfo *addrhead = NULL, *addr;
    char *proto, *family;
    char buf[128];

    /*
     * if file can't be opened, then assume that it's not available and
     * that the caller should just fall back to the old nfsctl interface
      */
    fd = open(DNFSD_PORTS_FILE, O_WRONLY);
    if (fd < 0) {
        xlog(L_ERROR, "couldn't open ports file: errno "
                      "%d (%m)", errno);
        goto error;
    }

    rc = getaddrinfo(node, port, &hints, &addrhead);
    if (rc == EAI_NONAME && !strcmp(port, "nfs")) {
        snprintf(buf, sizeof(buf), "%d", DNFS_PORT);
        rc = getaddrinfo(node, buf, &hints, &addrhead);
    }

    if (rc != 0) {
        xlog(L_ERROR, "unable to resolve %s:%s: %s",
             node ? node : "ANYADDR", port,
             rc == EAI_SYSTEM ? strerror(errno) :
             gai_strerror(rc));
        goto error;
    }

    addr = addrhead;
    while(addr) {
        /* skip non-TCP / non-UDP sockets */
        switch(addr->ai_protocol) {
            case IPPROTO_UDP:
                proto = "UDP";
                break;
            case IPPROTO_TCP:
                proto = "TCP";
                break;
            default:
                addr = addr->ai_next;
                continue;
        }

        switch(addr->ai_addr->sa_family) {
            case AF_INET:
                family = "AF_INET";
                break;
#ifdef IPV6_SUPPORTED
            case AF_INET6:
                family = "AF_INET6";
                break;
#endif /* IPV6_SUPPORTED */
            default:
                addr = addr->ai_next;
                continue;
        }

        /* open socket and prepare to hand it off to kernel */
        sockfd = socket(addr->ai_family, addr->ai_socktype,
                        addr->ai_protocol);
        if (sockfd < 0) {
            if (errno != EAFNOSUPPORT) {
                xlog(L_ERROR, "unable to create %s %s socket: "
                              "errno %d (%m)", family, proto, errno);
                rc = errno;
                goto error;
            }
            addr = addr->ai_next;
            continue;
        }

        xlog(D_GENERAL, "Created %s %s socket.", family, proto);

#ifdef IPV6_SUPPORTED
        if (addr->ai_family == AF_INET6 &&
        setsockopt(sockfd, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on))) {
        xlog(L_ERROR, "unable to set IPV6_V6ONLY: "
                      "errno %d (%m)\n", errno);
        rc = errno;
        goto error;
    }
#endif /* IPV6_SUPPORTED */
        if (addr->ai_protocol == IPPROTO_TCP &&
            setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) {
            xlog(L_ERROR, "unable to set SO_REUSEADDR on %s "
                          "socket: errno %d (%m)", family, errno);
            rc = errno;
            goto error;
        }

        if (bind(sockfd, addr->ai_addr, addr->ai_addrlen)) {
            xlog(L_ERROR, "unable to bind %s %s socket: "
                          "errno %d (%m)", family, proto, errno);
            rc = errno;
            goto error;
        }
        if (addr->ai_protocol == IPPROTO_TCP && listen(sockfd, 64)) {
            xlog(L_ERROR, "unable to create listening socket: "
                          "errno %d (%m)", errno);
            rc = errno;
            goto error;
        }

        if (dnfssvc_write_ports(sockfd) != SUCCESS) {
            /*
             * this error may be common on older kernels that don't
             * support IPv6, so turn into a debug message.
             */
            if (errno == EAFNOSUPPORT)
                fac = D_ALL;
            xlog(fac, "writing fd to kernel failed: errno %d (%m)",
                 errno);
            rc = errno;
            goto error;
        } else {
            // TODO 这里需要处理成连续写入，第一次写入应该清空，最后结束的时候写入到文件
            snprintf(buf, sizeof(buf), "%d\n", sockfd);
            write(fd, buf, strlen(buf));
        }

        close(fd);
        close(sockfd);
        sockfd = fd = -1;
        addr = addr->ai_next;
    }
    error:
    if (fd >= 0)
        close(fd);
    if (sockfd >= 0)
        close(sockfd);
    if (addrhead)
        freeaddrinfo(addrhead);
    return rc;
}

/*
 * 设置nfsd的进程数量并启动进程内容
 */
int
dnfssvc_threads(unsigned short port, const int nrservs)
{
	struct nfsctl_arg	arg;
	struct servent *ent;
	ssize_t n;
	int fd;
    char buf[128];

	fd = open(DNFSD_THREAD_FILE, O_WRONLY);
    if (fd < 0) {
        xlog(L_ERROR, "Failed to open nfs thread file: %m");
        return errno;
    }

    /* 2.5+ kernel with nfsd filesystem mounted.
     * Just write the number of threads.
     */
    struct net *net = netns(file);
    if (nrservs < 0)
        return -EINVAL;
    int rv = nfsd_svc(nrservs, net, file->f_cred);
    if (rv < 0)
        return rv;

    snprintf(buf, sizeof(buf), "%d\n", nrservs);
    write(fd, buf, strlen(buf));
    close(fd);

    return SUCCESS;

    // 下面的代码使用老版本的syscall启动，目的是兼容，新版本不再使用syscall，因此删除
    //	if (!port) {
    //		ent = getservbyname("nfs", "udp");
    //		if (ent != NULL)
    //			port = ntohs(ent->s_port);
    //		else
    //			port = DNFS_PORT;
    //	}
    //
    //	arg.ca_version = DNFSCTL_VERSION;
    //	arg.ca_svc.svc_nthreads = nrservs;
    //	arg.ca_svc.svc_port = port;
    //	return nfsctl(DNFSCTL_SVC, &arg, NULL);
}
