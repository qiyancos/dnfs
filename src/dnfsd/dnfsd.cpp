/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Rock Lee lsk_mprc@pku.edu.cn
 * Contributor(nfs-ganesha): Philippe DENIEL   philippe.deniel@cea.fr
 *                Thomas LEIBOVICI  thomas.leibovici@cea.fr
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

#include <unistd.h>
#include <string>
#include <iostream>

#include "log/log.h"
#include "utils/thread_utils.h"
#include "dnfsd/dnfsd.h"
#include "dnfsd/dnfs_ntirpc.h"
#include "dnfsd/dnfs_config.h"
#include "dnfsd/dnfs_init.h"

using namespace std;

#define MODULE_NAME "main"

// 服务默认选项字符串信息
static const char options[] = "v@L:N:f:p:FRTE:ChI:x";

// 服务帮助信息
static const char usage[] =
    "Usage: %s [-hd][-L <logfile>][-N <dbg_lvl>][-f <config_file>]\n"
	"\t[-v]                display version information\n"
	"\t[-L <logfile>]      set the default logfile for the daemon\n"
	"\t[-N <dbg_lvl>]      set the verbosity level\n"
	"\t[-f <config_file>]  set the config file to be used\n"
	"\t[-p <pid_file>]     set the pid file\n"
	"\t[-F]                the program stays in foreground\n"
	"\t[-R]                daemon will manage RPCSEC_GSS (default is no RPCSEC_GSS)\n"
	"\t[-T]                dump the default configuration on stdout\n"
	"\t[-E <epoch>]        overrides ServerBootTime for ServerEpoch\n"
	"\t[-I <nodeid>]       cluster nodeid\n"
	"\t[-C]                dump trace when segfault\n"
	"\t[-x]                fatal exit if there are config errors on startup\n"
	"\t[-h]                display this help\n"
	"----------------- Signals ----------------\n"
	"SIGHUP     : Reload LOG and EXPORT config\n"
	"SIGTERM    : Cleanly terminate the program\n"
	"------------- Default Values -------------\n"
	"LogFile    : SYSLOG\n"
	"PidFile    : " DNFSD_PIDFILE_PATH "\n"
	"DebugLevel : NIV_EVENT\n" "ConfigFile : " DNFSD_CONFIG_PATH " \n";

// 默认的配置文件路径
string nfs_config_path = DNFSD_CONFIG_PATH;

// 默认的PID文件路径
[[maybe_unused]] string nfs_pidfile_path = DNFSD_PIDFILE_PATH;

/* TODO */
[[maybe_unused]] time_t nfs_ServerEpoch;

// debug日志的级别
log_level_t debug_level = LNOLOG;

// 是否常驻后台的形式运行
bool detach_flag = true;

/* 针对配置文件严重错误是否退出进程 */
[[maybe_unused]] bool config_errors_fatal;

// 是否打印过程日志信息
bool dump_trace = false;

// 主程序名称
string exec_name;

// 主机名称HOSTNAME
string nfs_host_name = "localhost";

// 日志文件完整路径
string log_path;

// 主程序运行参数解析
static void arg_parser(int argc, char** argv) {

    // 获取主程序名称
    exec_name = strrchr(argv[0], '/') + 1;
    if (exec_name[0] == '\0')
        exec_name = argv[0];

    // 获取主机名称HOSTNAME
    nfs_host_name = "localhost";
    char localmachine[MAXHOSTNAMELEN + 1];
    if (gethostname(localmachine, sizeof(localmachine)) != 0) {
        cerr << "Could not get local host name, exiting..." << endl;
        exit(1);
    } else {
        nfs_host_name = localmachine;
    }

    int c;
    /* now parsing options with getopt */
    while ((c = getopt(argc, argv, options)) != EOF) {
        switch (c) {
            case 'v':
            case '@':
                printf("NFS-Ganesha Release = V%s\n", DNFSD_VERSION);
                /* A little backdoor to keep track of binary versions */
                printf("%s compiled on %s at %s\n", exec_name.c_str(),
                       __DATE__, __TIME__);
                printf("Release comment = %s\n", VERSION_COMMENT);
                printf("Git HEAD = %s\n", _GIT_HEAD_COMMIT);
                printf("Git Describe = %s\n", _GIT_DESCRIBE);
                exit(0);

            case 'L':
                /* Default Log */
                log_path = optarg;
                break;

            case 'N':
                /* debug level */
                debug_level = logger.decode_log_level(optarg);
                break;

            case 'f':
                /* config file */
                nfs_config_path = optarg;
                break;

            case 'p':
                /* PID file */
                nfs_pidfile_path = optarg;
                break;

            case 'F':
                /* Don't detach, foreground mode */
                detach_flag = false;
                break;

            case 'R':
                /* Shall we manage  RPCSEC_GSS ? */
                fprintf(stderr,
                        "\n\nThe -R flag is deprecated, use this syntax"
                        " in the configuration file instead:\n\n");
                fprintf(stderr, "NFS_KRB5\n");
                fprintf(stderr, "{\n");
                fprintf(stderr,
                        "\tPrincipalName = nfs@<your_host> ;\n");
                fprintf(stderr, "\tKeytabPath = /etc/krb5.keytab ;\n");
                fprintf(stderr, "\tActive_krb5 = true ;\n");
                fprintf(stderr, "}\n\n\n");
                exit(1);

            case 'T':
                /* Dump the default configuration on stdout */
                nfs_start_info.dump_default_config = true;
                break;

            case 'C':
                dump_trace = true;
                break;

            case 'E':
                nfs_ServerEpoch = (time_t) atoll(optarg);
                break;

            case 'x':
                config_errors_fatal = true;
                break;

            case 'h':
                fprintf(stderr, usage, exec_name.c_str());
                exit(0);

            default: /* '?' */
                fprintf(stderr, "Try '%s -h' for usage\n",
                        exec_name.c_str());
                exit(1);
        }
    }
}

/* 该函数用于处理程序退出的时候需要执行的操作 */
void exit_process(const int exit_code) {
    exit(exit_code);
}

// 主程序入口
int main(int argc, char ** argv)
{
    // 解析主程序参数并初始化部分状态变量
    arg_parser(argc, argv);

    // 初始化配置文件解析处理
    init_config(nfs_config_path);

    // 初始化日志
    logger.set_exit_func(-1, exit_process);
    init_logging(exec_name, nfs_host_name, debug_level,
                 detach_flag, log_path);

    // 设置当前线程名称
    ThreadPool::set_thread_name(exec_name + "_main");

    // 初始化崩溃信号处理函数hook
    if (dump_trace) {
        init_crash_handlers();
    }

    // 注册tirpc的处理操作参数
    init_ntirpc_settings();

    /* 检查malloc功能的可用性 */
    init_check_malloc();

	/* 确定是否在后台以守护进程的形式执行 */
	if (detach_flag) {
        /* 调用系统函数自动转化为daemon运行模式
         * 当nochdir为零时，将当前目录变为根目录，否则不变，当noclose为零时，标准输入、
         * 标准输出和错误输出重导向为/dev/null不输出任何信息，否则照样输出。 */
        if (daemon(0, 0)) {
            LOG(MODULE_NAME, L_ERROR,
                     "Error detaching process from parent: %s",
                     strerror(errno));
        }
    }

    /* 初始化线程对信号的处理操作 */
    init_thread_signal_mask();

	/* Everything seems to be OK! We can now start service threads */
	dnfs_start();

	return 0;
}
