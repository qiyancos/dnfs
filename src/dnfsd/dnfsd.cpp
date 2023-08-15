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

#include "utils/thread_utils.h"
#include "utils/rpc_mem_util.h"
#include "utils/log.h"
#include "dnfsd/dnfsd.h"

extern "C" {
#include "dnfsd/config.h"
#include "dnfsd/dnfs_init.h"
}

using namespace std;

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
	"PidFile    : " GANESHA_PIDFILE_PATH "\n"
	"DebugLevel : NIV_EVENT\n" "ConfigFile : " GANESHA_CONFIG_PATH " \n";

// 服务启动配置信息
static nfs_start_info_t my_nfs_start_info = {
        .dump_default_config = false,
        .lw_mark_trigger = false,
        .drop_caps = true
};

// 默认的配置文件路径
string nfs_config_path = GANESHA_CONFIG_PATH;

// 默认的PID文件路径
string nfs_pidfile_path = GANESHA_PIDFILE_PATH;

// TODO
time_t nfs_ServerEpoch = 0;

// 是否在出现致命错误日志的时候直接退出程序
bool config_errors_fatal = false;

// debug日志的级别 todo
int debug_level = -1;

// 是否常驻后台的形式运行
bool detach_flag = true;

// 是否打印过程日志信息
bool dump_trace = false;

// 主程序名称
string exec_name;

// 主机名称HOSTNAME
string nfs_host_name = "localhost";

// 日志文件完整路径
string log_path;

// tirpc的控制参数集合
tirpc_pkg_params ntirpc_pp = {
    TIRPC_DEBUG_FLAG_DEFAULT,
    0,
    ThreadPool::set_thread_name,
    NULL, // 打印错误日志使用的函数 void(char* fmt, ...)
    rpc_free,
    rpc_malloc,
    NULL, // 包含对齐的内存空间分配函数
    NULL, // 根据给定的结构体个数和结构体大小分配空间
    NULL, // 根据新的大小对之前已经分配的内存区域进行重新分配，新的区域会复制之前区域的数据
};

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
                printf("NFS-Ganesha Release = V%s\n", GANESHA_VERSION);
                /* A little backdoor to keep track of binary versions */
                printf("%s compiled on %s at %s\n", exec_name.c_str(),
                       __DATE__, __TIME__);
                printf("Release comment = %s\n", VERSION_COMMENT);
                printf("Git HEAD = %s\n", _GIT_HEAD_COMMIT);
                printf("Git Describe = %s\n", _GIT_DESCRIBE);
                exit(0);
                break;

            case 'L':
                /* Default Log */
                log_path = optarg;
                break;

            case 'N':
                /* debug level */
                // TODO
                debug_level = ReturnLevelAscii(optarg);
                if (debug_level == -1) {
                    fprintf(stderr,
                            "Invalid value for option 'N': NIV_NULL, NIV_MAJ, NIV_CRIT, NIV_EVENT, NIV_DEBUG, NIV_MID_DEBUG or NIV_FULL_DEBUG expected.\n");
                    exit(1);
                }
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
                        "\n\nThe -R flag is deprecated, use this syntax in the configuration file instead:\n\n");
                fprintf(stderr, "NFS_KRB5\n");
                fprintf(stderr, "{\n");
                fprintf(stderr,
                        "\tPrincipalName = nfs@<your_host> ;\n");
                fprintf(stderr, "\tKeytabPath = /etc/krb5.keytab ;\n");
                fprintf(stderr, "\tActive_krb5 = true ;\n");
                fprintf(stderr, "}\n\n\n");
                exit(1);
                break;

            case 'T':
                /* Dump the default configuration on stdout */
                my_nfs_start_info.dump_default_config = true;
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
                fprintf(stderr, "Try '%s -h' for usage\n", exec_name.c_str());
                exit(1);
        }
    }
}

// 主程序入口
int main(int argc, char ** argv)
{
    // 设置当前线程名称
    ThreadPool::set_thread_name(exec_name + "_main");

    // 解析主程序参数并初始化部分状态变量
    arg_parser(argc, argv);

    // 初始化日志
    Logger& logger = Logger::init(exec_name + "_main", nfs_host_name);

    // 初始化崩溃信号处理函数hook
    if (dump_trace) {
        init_crash_handlers();
    }

    // 注册tirpc的处理操作参数
    if (!tirpc_control(TIRPC_PUT_PARAMETERS, &ntirpc_pp)) {
        LogFatal(COMPONENT_INIT, "Setting nTI-RPC parameters failed");
    }

    dnfs_prereq_init(exec_name, nfs_host_name debug_level, log_path, dump_trace);

	/* initialize nfs_init */
	nfs_init_init();
    /* Check if malloc function is valid */
	nfs_check_malloc();

	/* Start in background, if wanted */
	if (detach_flag) {
        /* daemonize the process (fork, close xterm fds,
 * detach from parent process) */
        if (daemon(0, 0))
            LogFatal(COMPONENT_MAIN,
                     "Error detaching process from parent: %s",
                     strerror(errno));

        /* In the child process, change the log header
         * if not, the header will contain the parent's pid */
        set_const_log_str();
    }

	/* Make sure Linux file i/o will return with error
	 * if file size is exceeded. */
#ifdef _LINUX
	signal(SIGXFSZ, SIG_IGN);
#endif
    sigset_t signals_to_block;

	/* Set up for the signal handler.
	 * Blocks the signals the signal handler will handle.
	 */
	sigemptyset(&signals_to_block);
	sigaddset(&signals_to_block, SIGTERM);
	sigaddset(&signals_to_block, SIGHUP);
	sigaddset(&signals_to_block, SIGPIPE);
	if (pthread_sigmask(SIG_BLOCK, &signals_to_block, NULL) != 0) {
		LogFatal(COMPONENT_MAIN,
			 "Could not start nfs daemon, pthread_sigmask failed");
			goto fatal_die;
	}

	/* Everything seems to be OK! We can now start service threads */
	nfs_start(&my_nfs_start_info);

	if (log_path)
		free(log_path);

	return 0;

fatal_die:
	if (log_path)
		free(log_path);
	if (pidfile != -1)
		close(pidfile);

	/* systemd journal won't display our errors without this */
	sleep(1);

	LogFatal(COMPONENT_INIT,
		 "Fatal errors.  Server exiting...");
	/* NOT REACHED */
	return 2;
}
