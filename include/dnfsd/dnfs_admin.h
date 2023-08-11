//
// Created by iecas on 2023/8/11.
//

#ifndef DNFS_DNFS_ADMIN_THREAD_H
#define DNFS_DNFS_ADMIN_THREAD_H

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

void admin_halt(void);
void *admin_thread(void *UnusedArg);
void nfs_Init_admin_thread(void);

#endif //DNFS_DNFS_ADMIN_THREAD_H
