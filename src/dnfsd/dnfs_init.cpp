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

extern "C" {
#include "log/log.h"
}

#include "dnfsd/dnfsd.h"

using namespace std;

static void crash_handler(int signo, siginfo_t *info, void *ctx)
{
    gsh_backtrace();
    /* re-raise the signal for the default signal handler to dump core */
    raise(signo);
}

static void install_sighandler(int signo,
                               void (*handler)(int, siginfo_t *, void *)) {
    struct sigaction sa = {};
    int ret;

    sa.sa_sigaction = handler;
    /* set SA_RESETHAND to restore default handler */
    sa.sa_flags = SA_SIGINFO | SA_RESETHAND | SA_NODEFER;

    sigemptyset(&sa.sa_mask);

    ret = sigaction(signo, &sa, NULL);
    if (ret) {
        LogWarn(COMPONENT_INIT,
                "Install handler for signal (%s) failed",
                strsignal(signo));
    }
}

void init_crash_handlers(void) {
    install_sighandler(SIGSEGV, crash_handler);
    install_sighandler(SIGABRT, crash_handler);
    install_sighandler(SIGBUS, crash_handler);
    install_sighandler(SIGILL, crash_handler);
    install_sighandler(SIGFPE, crash_handler);
    install_sighandler(SIGQUIT, crash_handler);
}