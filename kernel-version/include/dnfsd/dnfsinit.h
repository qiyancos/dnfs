/*
 *   utils/nfsd/nfssvc.h -- nfs service control routines for rpc.nfsd
 *
 *   Copyright (C) 2009 Red Hat, Inc <nfs@redhat.com>.
 *   Copyright (C) 2009 Jeff Layton <jlayton@redhat.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 0211-1301 USA
 *
 */

#ifndef DNFSD_FS_DIR
#define DNFSD_FS_DIR	  "/proc/fs/dnfsd"
#endif

#define DNFSD_PORTS_FILE   DNFSD_FS_DIR "/portlist"
#define DNFSD_VERS_FILE    DNFSD_FS_DIR "/versions"
#define DNFSD_THREAD_FILE  DNFSD_FS_DIR "/threads"

#define netns(file) file_inode(file)->i_sb->s_fs_info

int dnfsd_init();

int dnfssvc_create_status_dir();
int	dnfssvc_inuse(void);
int	dnfssvc_set_time(const char *type, const int seconds);
int	dnfssvc_set_sockets(const unsigned int protobits,
                           const char *node, const char *port);
int dnfssvc_setvers(unsigned int ctlbits, unsigned int minorvers4);
int	dnfssvc_threads(unsigned short port, int nrservs);
