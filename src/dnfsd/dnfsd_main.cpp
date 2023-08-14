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
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the MIT lisence for
 * more details. You should have received a copy of the MIT License
 * along with this project.
 *
 */

#include "log/log.h"
#include "dnfsd/config.h"
#include "dnfsd/dnfsd.h"
#include "dnfsd/dnfs_init.h"

char *nfs_config_path = GANESHA_CONFIG_PATH;
char *nfs_pidfile_path = GANESHA_PIDFILE_PATH;

static nfs_start_info_t my_nfs_start_info = {
        .dump_default_config = false,
        .lw_mark_trigger = false,
        .drop_caps = true
};

static const char options[] = "v@L:N:f:p:FRTE:ChI:x";
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

/**
 * main: simply the main function.
 *
 * The 'main' function as in every C program.
 *
 * @param argc number of arguments
 * @param argv array of arguments
 *
 * @return status to calling program by calling the exit(3C) function.
 *
 */
int main(int argc, char *argv[])
{
	char *tempo_exec_name = NULL;
	char localmachine[MAXHOSTNAMELEN + 1];
	int c;
	int pidfile = -1;               /* fd for file to store pid */
	char *log_path = NULL;
	char *exec_name = "nfs-ganesha";
    char *nfs_host_name = "localhost";
	int debug_level = -1;
	int detach_flag = true;
	bool dump_trace = false;
	sigset_t signals_to_block;

	tempo_exec_name = strrchr(argv[0], '/');
	if (tempo_exec_name != NULL)
		exec_name = strdup(tempo_exec_name + 1);

	if (*exec_name == '\0')
		exec_name = argv[0];

	/* get host name */
	if (gethostname(localmachine, sizeof(localmachine)) != 0) {
		fprintf(stderr, "Could not get local host name, exiting...\n");
		exit(1);
	} else {
		nfs_host_name = main_strdup("host_name", localmachine);
	}

	/* now parsing options with getopt */
	while ((c = getopt(argc, argv, options)) != EOF) {
		switch (c) {
		case 'v':
		case '@':
			printf("NFS-Ganesha Release = V%s\n", GANESHA_VERSION);
#if !GANESHA_BUILD_RELEASE
			/* A little backdoor to keep track of binary versions */
			printf("%s compiled on %s at %s\n", exec_name, __DATE__,
			       __TIME__);
			printf("Release comment = %s\n", VERSION_COMMENT);
			printf("Git HEAD = %s\n", _GIT_HEAD_COMMIT);
			printf("Git Describe = %s\n", _GIT_DESCRIBE);
#endif
			exit(0);
			break;

		case 'L':
			/* Default Log */
			log_path = main_strdup("log_path", optarg);
			break;

		case 'N':
            /* debug level */
            debug_level = ReturnLevelAscii(optarg);
            if (debug_level == -1) {
                fprintf(stderr,
                        "Invalid value for option 'N': NIV_NULL, NIV_MAJ, NIV_CRIT, NIV_EVENT, NIV_DEBUG, NIV_MID_DEBUG or NIV_FULL_DEBUG expected.\n");
                exit(1);
            }
            break;

		case 'f':
			/* config file */
			nfs_config_path = main_strdup("config_path", optarg);
			break;

		case 'p':
			/* PID file */
			nfs_pidfile_path = main_strdup("pidfile_path", optarg);
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
			break;

		case 'C':
			dump_trace = true;
			break;

		case 'E':
		case 'I':
		case 'x':
			break;

		case 'h':
			fprintf(stderr, usage, exec_name);
			exit(0);

		default: /* '?' */
			fprintf(stderr, "Try '%s -h' for usage\n", exec_name);
			exit(1);
		}
	}

    /* initialize memory and logging */
    nfs_prereq_init(exec_name, nfs_host_name, debug_level, log_path,
                    dump_trace);

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

	if (tempo_exec_name)
		free(exec_name);
	if (log_path)
		free(log_path);

	return 0;

fatal_die:
	if (tempo_exec_name)
		free(exec_name);
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
