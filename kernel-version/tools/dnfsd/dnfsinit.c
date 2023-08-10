/*
 * utils/nfsd/nfssvc.c
 *
 * Run an DNFS daemon.
 *
 * Copyright (C) 1995, 1996 Olaf Kirch <okir@monad.swb.de>
 */

//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netdb.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <sys/stat.h>
//#include <unistd.h>
//#include <fcntl.h>
//#include <errno.h>
//#include <stdlib.h>
//#include <string.h>
//#include <stdarg.h>

#include "utils/xlog.h"
#include "dnfsd/dnfs.h"
#include "dnfsd/dnfsd.h"
#include "dnfsd/nfslib.h"
#include "dnfsd/dnfsinit.h"
#include "dnfsd/dnfsctl.h"

static int nfsd_init_net(struct net *net)
{
    int retval;
    struct nfsd_net *nn = net_generic(net, nfsd_net_id);

    retval = nfsd_export_init(net);
    if (retval)
        goto out_export_error;
    retval = nfsd_idmap_init(net);
    if (retval)
        goto out_idmap_error;
    nn->nfsd_versions = NULL;
    nn->nfsd4_minorversions = NULL;
    retval = nfsd_reply_cache_init(nn);
    if (retval)
        goto out_drc_error;
    nn->nfsd4_lease = 90;	/* default lease time */
    nn->nfsd4_grace = 90;
    nn->somebody_reclaimed = false;
    nn->track_reclaim_completes = false;
    nn->clverifier_counter = prandom_u32();
    nn->clientid_base = prandom_u32();
    nn->clientid_counter = nn->clientid_base + 1;
    nn->s2s_cp_cl_id = nn->clientid_counter++;

    atomic_set(&nn->ntf_refcnt, 0);
    init_waitqueue_head(&nn->ntf_wq);
    seqlock_init(&nn->boot_lock);

    return 0;

    out_drc_error:
    nfsd_idmap_shutdown(net);
    out_idmap_error:
    out_export_error:
    return retval;
}

static void nfsd_exit_net(struct net *net)
{
    struct nfsd_net *nn = net_generic(net, nfsd_net_id);

    nfsd_reply_cache_shutdown(nn);
    nfsd_idmap_shutdown(net);
    nfsd_netns_free_versions(net_generic(net, nfsd_net_id));
}

static int nfsd_init_fs_context(struct fs_context *fc)
{
    put_user_ns(fc->user_ns);
    fc->user_ns = get_user_ns(fc->net_ns->user_ns);
    fc->ops = &nfsd_fs_context_ops;
    return 0;
}

static struct file_system_type nfsd_fs_type = {
        .owner		= NULL,
        .name		= "nfsd",
        .init_fs_context = nfsd_init_fs_context,
        .kill_sb	= nfsd_umount,
};

static struct pernet_operations nfsd_net_ops = {
        .init = nfsd_init_net,
        .exit = nfsd_exit_net,
        .id   = &nfsd_net_id,
        .size = sizeof(struct nfsd_net),
};

/*
 * dnfsd模块初始化主函数
 */
int
dnfsd_init() {
    int retval;

    // 执行内核错误注入处理操作，基于虚拟化的文件进行debug，通过读操作绑定函数
    // 可以实时查看系统运行时信息或者使用写操作；通过写操作绑定函数，实现相关的
    // 错误处理或者状态处理操作操作。
    // nfsd_fault_inject_init(); /* nfsd fault injection controls */

    // 注册和初始化统计相关的信息，统计信息设置了一个默认的描述符文件存放统计的相关信息
    // 涉及所有统计信息的更新都是在各个功能函数中处理更新的，这里的注册只是注册了一个读取功能
    // 到对应的描述符文件中，允许通过读取直接获取nfs的统计信息
    // nfsd_stat_init();	/* Statistics */

    // 为NFS创建一个缓存结构，用于后续缓存的生成和使用，缓存用于存储特定的数据结构
    // 每次都会申请都会获得一个该数据结构的空间来处理对应的内容
    retval = nfsd_drc_slab_create();
    if (retval)
        goto out_free_stat;

    // 将实际的lockd回调函数注册到lockd对应的头文件中，lockd类似于一个文件锁
    // 该锁定为rpc的请求提供一个远程与本地的共享锁
    nfsd_lockd_init();	/* lockd->nfsd callbacks */

    // 在操作系统中注册一个新的文件系统信息
    retval = register_filesystem(&nfsd_fs_type);
    if (retval)
        goto out_free_exports;

    // 针对nfsd添加一个专属的网络命名空间，并将这个空间追加到网络命名空间的链表尾部
    // 常用的net指针就是当前进程所在网络命名空间的指针，通过net_generic函数可以提取出
    // 当前网络空间id(nfsd_net_id)对应的专门的网络空间信息结构体指针(struct nfsd_net)
    retval = register_pernet_subsys(&nfsd_net_ops);
    if (retval < 0)
        goto out_free_filesystem;

    return 0;

out_free_filesystem:
    unregister_filesystem(&nfsd_fs_type);
out_free_exports:
    nfsd_lockd_shutdown();
    nfsd_drc_slab_free();
out_free_stat:
    nfsd_stat_shutdown();
    nfsd_fault_inject_cleanup();
    return retval;
}

/*
 * 退出函数，对注册的信息取消相关的注册内容
 */
void
exit_nfsd(void)
{
    //unregister_pernet_subsys(&nfsd_net_ops);
    nfsd_drc_slab_free();
    nfsd_stat_shutdown();
    nfsd_lockd_shutdown();
    nfsd_fault_inject_cleanup();
    unregister_filesystem(&nfsd_fs_type);
}

/*
 * 创建DNFS的运行时目录
 */
int
dnfssvc_create_status_dir()
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
        cmd = DNFSCTL_VERISSET(ctlbits, n) ? NFSD_SET : NFSD_CLEAR;
        nfsd_vers(nn, n, cmd);
        off += snprintf(buf+off, sizeof(buf) - off, "%c%d ",
                        DNFSCTL_VERISSET(ctlbits, n) ? '+' : '-', n);
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

    // 将NFS不同版本对应的入口函数注册到SUNRPC服务器中
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
