/*
 * nfsd
 *
 * This is the user level part of nfsd. This is very primitive, because
 * all the work is now done in the kernel module.
 *
 * Copyright (C) 1995, 1996 Olaf Kirch <okir@monad.swb.de>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <libgen.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "nfslib.h"
#include "nfssvc.h"
#include "xlog.h"

#ifndef NFSD_NPROC
#define NFSD_NPROC 8
#endif

static void	usage(const char *);

static struct option longopts[] =
{
	{ "host", 1, 0, 'H' },
	{ "help", 0, 0, 'h' },
	{ "no-nfs-version", 1, 0, 'N' },
	{ "nfs-version", 1, 0, 'V' },
	{ "no-tcp", 0, 0, 'T' },
	{ "no-udp", 0, 0, 'U' },
	{ "port", 1, 0, 'P' },
	{ "port", 1, 0, 'p' },
	{ "debug", 0, 0, 'd' },
	{ "syslog", 0, 0, 's' },
	{ "rdma", 2, 0, 'R' },
	{ "grace-time", 1, 0, 'G'},
	{ "lease-time", 1, 0, 'L'},
	{ NULL, 0, 0, 0 }
};

int
main(int argc, char **argv)
{
	int	count = NFSD_NPROC, c, i, error = 0, portnum = 0, fd, found_one;
	char *p, *progname, *port, *rdma_port = NULL;
	char **haddr = NULL;
	int hcounter = 0;
	int	socket_up = 0;
	unsigned int minorvers = 0;
	unsigned int minorversset = 0;
	unsigned int versbits = NFSCTL_VERDEFAULT;
	unsigned int protobits = NFSCTL_ALLBITS;
	int grace = -1;
	int lease = -1;

	progname = strdup(basename(argv[0]));
	if (!progname) {
		fprintf(stderr, "%s: unable to allocate memory.\n", argv[0]);
		exit(1);
	}

	port = strdup("nfs");
	if (!port) {
		fprintf(stderr, "%s: unable to allocate memory.\n", progname);
		exit(1);
	}

	haddr = malloc(sizeof(char *));
	if (!haddr) {
		fprintf(stderr, "%s: unable to allocate memory.\n", progname);
		exit(1);
	}
	haddr[0] = NULL;

	xlog_syslog(0);
	xlog_stderr(1);

    // 解析参数并生成相应的初始化参数
	while ((c = getopt_long(argc, argv, "dH:hN:V:p:P:sTUrG:L:", longopts, NULL)) != EOF) {
		switch(c) {
		case 'd':
            // 如果开启debug模式，那么所有类型的信息都会被打印出来
			xlog_config(D_ALL, 1);
			break;
		case 'H':
            // 支持指定多个Host作为启动服务的监听IP地址
			if (hcounter) {
				haddr = realloc(haddr, sizeof(char*) * hcounter+1);
				if(!haddr) {
					fprintf(stderr, "%s: unable to allocate "
							"memory.\n", progname);
					exit(1);
				}
			}
			haddr[hcounter] = strdup(optarg);
			if (!haddr[hcounter]) {
				fprintf(stderr, "%s: unable to allocate "
					"memory.\n", progname);
				exit(1);
			}
			hcounter++;
			break;
		case 'P':	/* XXX for nfs-server compatibility */
		case 'p':
            // 指定当前服务器启动所监听的端口号默认为"nfs"，系统会在
            // /etc/services下面找到对应服务的默认端口号
			/* only the last -p option has any effect */
			portnum = atoi(optarg);
			if (portnum <= 0 || portnum > 65535) {
				fprintf(stderr, "%s: bad port number: %s\n",
					progname, optarg);
				usage(progname);
			}
			free(port);
			port = strdup(optarg);
			if (!port) {
				fprintf(stderr, "%s: unable to allocate "
						"memory.\n", progname);
				exit(1);
			}
			break;
		case 'r':
            // 默认启动的时候并不会使用rdma加速
			rdma_port = "nfsrdma";
			break;
		case 'R': /* --rdma */
			if (optarg)
				rdma_port = optarg;
			else
				rdma_port = "nfsrdma";
			break;

		case 'N':
            // 禁用指定的版本，只能对2、3、4的大版本进行操作
			switch((c = strtol(optarg, &p, 0))) {
			case 4:
				if (*p == '.') {
					int i = atoi(p+1);
					if (i > NFS4_MAXMINOR) {
						fprintf(stderr, "%s: unsupported minor version\n", optarg);
						exit(1);
					}
					NFSCTL_VERSET(minorversset, i);
					NFSCTL_VERUNSET(minorvers, i);
					break;
				}
			case 3:
			case 2:
				NFSCTL_VERUNSET(versbits, c);
				break;
			default:
				fprintf(stderr, "%s: Unsupported version\n", optarg);
				exit(1);
			}
			break;
		case 'V':
            // 启动指定的NFS协议版本
			switch((c = strtol(optarg, &p, 0))) {
			case 4:
				if (*p == '.') {
					int i = atoi(p+1);
					if (i > NFS4_MAXMINOR) {
						fprintf(stderr, "%s: unsupported minor version\n", optarg);
						exit(1);
					}
					NFSCTL_VERSET(minorversset, i);
					NFSCTL_VERSET(minorvers, i);
					break;
				}
			case 3:
			case 2:
				NFSCTL_VERSET(versbits, c);
				break;
			default:
				fprintf(stderr, "%s: Unsupported version\n", optarg);
				exit(1);
			}
			break;
		case 's':
            // 是否强制打印到syslog中，默认只会在运行期间切换到syslog
			xlog_syslog(1);
			xlog_stderr(0);
			break;
		case 'T':
            // TCP协议支持开关，默认自动依据系统端口占用情况启用，为占用就会启动
			NFSCTL_TCPUNSET(protobits);
			break;
		case 'U':
            // UDP协议支持开关，默认自动依据系统端口占用情况启用，为占用就会启动
			NFSCTL_UDPUNSET(protobits);
			break;
		case 'G':
            // TODO，grace时间，不确定用途
			grace = strtol(optarg, &p, 0);
			if (*p || grace <= 0) {
				fprintf(stderr, "%s: Unrecognized grace time.\n", optarg);
				exit(1);
			}
			break;
		case 'L':
            // TODO，lease时间，不确定用途
			lease = strtol(optarg, &p, 0);
			if (*p || lease <= 0) {
				fprintf(stderr, "%s: Unrecognized lease time.\n", optarg);
				exit(1);
			}
			break;
		default:
			fprintf(stderr, "Invalid argument: '%c'\n", c);
		case 'h':
			usage(progname);
		}
	}

    // 默认接受最后一个参数是一个数字，代表希望启动的进程实例个数
    // 如果不传参数默认为8，如果传参数为0表示停止nfsd服务
	if (optind < argc) {
		if ((count = atoi(argv[optind])) < 0) {
			/* insane # of servers */
			fprintf(stderr,
				"%s: invalid server count (%d), using 1\n",
				argv[0], count);
			count = 1;
		} else if (count == 0) {
			/*
			 * don't bother setting anything else if the threads
			 * are coming down anyway.
			 */
			socket_up = 1;
			goto set_threads;
		}
	}

    // 打开日志开关，从这里才可以使用xlog模块输出日志
	xlog_open(progname);

    // 基于输入的版本支持信息以及TCP和UDP开关情况，设置相应的标志位
	/* make sure that at least one version is enabled */
	found_one = 0;
	for (c = NFSD_MINVERS; c <= NFSD_MAXVERS; c++) {
		if (NFSCTL_VERISSET(versbits, c))
			found_one = 1;
	}
	if (!found_one) {
		xlog(L_ERROR, "no version specified");
		exit(1);
	}

	if (NFSCTL_VERISSET(versbits, 4) &&
	    !NFSCTL_TCPISSET(protobits)) {
		xlog(L_ERROR, "version 4 requires the TCP protocol");
		exit(1);
	}

	if (chdir(NFS_STATEDIR)) {
		xlog(L_ERROR, "chdir(%s) failed: %m", NFS_STATEDIR);
		exit(1);
	}

    // TODO, 将nfsd挂载到/proc/fs/nfsd中，具体作用未知
	/* make sure nfsdfs is mounted if it's available */
	nfssvc_mount_nfsdfs(progname);

    // 根据"/proc/fs/nfsd/portlist"文件是否有接口相关的内容来确定当前nfsd是否启动
    // 如果已经启动可以修改当前运行中的进程数量，但是要求运行同一个nfsd程序处理才能完成修改
	/* can only change number of threads if nfsd is already up */
	if (nfssvc_inuse()) {
		socket_up = 1;
		goto set_threads;
	}

	/*
	 * Must set versions before the fd's so that the right versions get
	 * registered with rpcbind. Note that on older kernels w/o the right
	 * interfaces, these are a no-op.
	 * Timeouts must also be set before ports are created else we get
	 * EBUSY.
	 */
    // 根据启动选项中的版本支持情况，将版本信息写入"/proc/fs/nfsd/versions"中
	nfssvc_setvers(versbits, minorvers, minorversset);
    // 设置grace和lease时间并写入到/proc/fs/nfsd下面的两个时间文件中
	if (grace > 0)
		nfssvc_set_time("grace", grace);
	if (lease  > 0)
		nfssvc_set_time("lease", lease);

	i = 0;
	do {
		error = nfssvc_set_sockets(protobits, haddr[i], port);
		if (!error)
			socket_up = 1;
	} while (++i < hcounter);

	if (rdma_port) {
		error = nfssvc_set_rdmaport(rdma_port);
		if (!error)
			socket_up = 1;
	}
set_threads:
	/* don't start any threads if unable to hand off any sockets */
	if (!socket_up) {
		xlog(L_ERROR, "unable to set any sockets for nfsd");
		goto out;
	}
	error = 0;

	/*
	 * KLUDGE ALERT:
	 * Some kernels let nfsd kernel threads inherit open files
	 * from the program that spawns them (i.e. us).  So close
	 * everything before spawning kernel threads.  --Chip
	 */
	fd = open("/dev/null", O_RDWR);
	if (fd == -1)
		xlog(L_ERROR, "Unable to open /dev/null: %m");
	else {
		/* switch xlog output to syslog since stderr is being closed */
		xlog_syslog(1);
		xlog_stderr(0);
		(void) dup2(fd, 0);
		(void) dup2(fd, 1);
		(void) dup2(fd, 2);
	}
	closeall(3);

	if ((error = nfssvc_threads(portnum, count)) < 0)
		xlog(L_ERROR, "error starting threads: errno %d (%m)", errno);
out:
	free(port);
	for(i=0; i < hcounter; i++)
		free(haddr[i]);
	free(haddr);
	free(progname);
	return (error != 0);
}

static void
usage(const char *prog)
{
	fprintf(stderr, "Usage:\n"
		"%s [-d|--debug] [-H hostname] [-p|-P|--port port]\n"
		"     [-N|--no-nfs-version version] [-V|--nfs-version version]\n"
		"     [-s|--syslog] [-T|--no-tcp] [-U|--no-udp] [-r|--rdma=]\n"
		"     [-G|--grace-time secs] [-L|--leasetime secs] nrservs\n",
		prog);
	exit(2);
}
