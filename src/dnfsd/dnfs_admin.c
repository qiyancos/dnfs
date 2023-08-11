// SPDX-License-Identifier: LGPL-3.0-or-later
/*
 * vim:noexpandtab:shiftwidth=8:tabstop=8:
 *
 * Copyright CEA/DAM/DIF  (2008)
 * contributeur : Philippe DENIEL   philippe.deniel@cea.fr
 *                Thomas LEIBOVICI  thomas.leibovici@cea.fr
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * ---------------------------------------
 */

/**
 * @file  nfs_admin_thread.c
 * @brief The admin_thread and support code.
 */

#include "dnfsd/dnfs_admin.h"
#include "dnfsd/dnfs_dispatcher.h"
#include "common_utils.h"

/**
 * @brief Mutex protecting shutdown flag.
 */

static pthread_mutex_t admin_control_mtx;

/**
 * @brief Condition variable to signal change in shutdown flag.
 */

static pthread_cond_t admin_control_cv;

void nfs_Init_admin_thread(void)
{
    PTHREAD_MUTEX_init(&admin_control_mtx, NULL);
    PTHREAD_COND_init(&admin_control_cv, NULL);
#ifdef USE_DBUS
    gsh_dbus_register_path("admin", admin_interfaces);
#endif				/* USE_DBUS */
    LogEvent(COMPONENT_NFS_CB, "Admin thread initialized");
}

/**
 * @brief Flag to indicate shutdown Ganesha.
 *
 * Protected by admin_control_mtx and signaled by admin_control_cv.
 */
bool admin_shutdown;

/**
 * @brief Signal the admin thread to shut down the system
 */

void admin_halt(void)
{
    PTHREAD_MUTEX_lock(&admin_control_mtx);

    if (!admin_shutdown) {
        admin_shutdown = true;
        pthread_cond_broadcast(&admin_control_cv);
    }

    PTHREAD_MUTEX_unlock(&admin_control_mtx);
    LogEvent(COMPONENT_MAIN, "NFS EXIT: %s done", __func__);
}

static void do_shutdown(void)
{
//    int rc = 0;
//    bool disorderly = false;

    LogEvent(COMPONENT_MAIN, "NFS EXIT: stopping NFS service");

    LogEvent(COMPONENT_MAIN, "Unregistering ports used by NFS service");
    /* finalize RPC package */
    Clean_RPC();

    LogEvent(COMPONENT_MAIN, "Shutting down RPC services");
    (void)svc_shutdown(SVC_SHUTDOWN_FLAG_NONE);

    PTHREAD_MUTEX_destroy(&admin_control_mtx);
    PTHREAD_COND_destroy(&admin_control_cv);
    LogEvent(COMPONENT_MAIN, "NFS EXIT: %s done", __func__);
}

void *admin_thread(void *UnusedArg)
{
    SetNameFunction("Admin");

    PTHREAD_MUTEX_lock(&admin_control_mtx);

    while (!admin_shutdown) {
        /* Wait for shutdown indication. */
        pthread_cond_wait(&admin_control_cv, &admin_control_mtx);
    }

    PTHREAD_MUTEX_unlock(&admin_control_mtx);

    do_shutdown();
    return NULL;
}