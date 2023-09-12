/*
 *
 * Copyright Reserved By All Project Contributors
 * Contributor: Rock Lee lsk_mprc@pku.edu.cn
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

#ifndef DNFSD_NFS_EXPORTS_H
#define DNFSD_NFS_EXPORTS_H

/* Constants for export options masks */
#define EXPORT_OPTION_FSID_SET 0x00000001 /* Set if Filesystem_id is set */
#define EXPORT_OPTION_USE_COOKIE_VERIFIER 0x00000002 /* Use cookie verifier */
/** Controls whether a directory's dirent cache is trusted for
    negative results. */
#define EXPORT_OPTION_TRUST_READIR_NEGATIVE_CACHE 0x00000008
#define EXPORT_OPTION_MAXREAD_SET 0x00000010 /* Set if MaxRead was specified */
#define EXPORT_OPTION_MAXWRITE_SET 0x00000020 /* Set if MaxWrite was
						 specified */
#define EXPORT_OPTION_PREFREAD_SET 0x00000040 /* Set if PrefRead was
						 specified */
#define EXPORT_OPTION_PREFWRITE_SET 0x00000080 /* Set if PrefWrite was
						  specified */
#define EXPORT_OPTION_SECLABEL_SET 0x00000100 /* Set if export supports v4.2
						 security labels */

/* Constants for export permissions masks */
#define EXPORT_OPTION_ROOT 0	/*< Allow root access as root uid */
#define EXPORT_OPTION_ROOT_ID_SQUASH 0x00000001	/*< Disallow root access as
						    root uid but preserve
						    alt_groups */
#define EXPORT_OPTION_ROOT_SQUASH 0x00000002	/*< Disallow root access as root
						    uid */
#define EXPORT_OPTION_ALL_ANONYMOUS 0x00000004	/*< all users are squashed to
						    anonymous */
#define EXPORT_OPTION_SQUASH_TYPES (EXPORT_OPTION_ROOT_SQUASH | \
				    EXPORT_OPTION_ROOT_ID_SQUASH | \
				    EXPORT_OPTION_ALL_ANONYMOUS) /*< All squash
								   types */
#define EXPORT_OPTION_ANON_UID_SET 0x00000008	/*< Indicates Anon_uid was set
						 */
#define EXPORT_OPTION_ANON_GID_SET 0x00000010	/*< Indicates Anon_gid was set
						 */
#define EXPORT_OPTION_READ_ACCESS 0x00000020	/*< R_Access= option specified
						 */
#define EXPORT_OPTION_WRITE_ACCESS 0x00000040	/*< RW_Access= option specified
						 */
#define EXPORT_OPTION_RW_ACCESS       (EXPORT_OPTION_READ_ACCESS     | \
				       EXPORT_OPTION_WRITE_ACCESS)
#define EXPORT_OPTION_MD_READ_ACCESS 0x00000080	/*< MDONLY_RO_Access= option
						    specified */
#define EXPORT_OPTION_MD_WRITE_ACCESS 0x00000100 /*< MDONLY_Access= option
						     specified */
#define EXPORT_OPTION_MD_ACCESS       (EXPORT_OPTION_MD_WRITE_ACCESS | \
				       EXPORT_OPTION_MD_READ_ACCESS)
#define EXPORT_OPTION_MODIFY_ACCESS   (EXPORT_OPTION_WRITE_ACCESS | \
				       EXPORT_OPTION_MD_WRITE_ACCESS)
#define EXPORT_OPTION_ACCESS_MASK     (EXPORT_OPTION_READ_ACCESS     | \
				       EXPORT_OPTION_WRITE_ACCESS    | \
				       EXPORT_OPTION_MD_WRITE_ACCESS | \
				       EXPORT_OPTION_MD_READ_ACCESS)

#endif //DNFSD_NFS_EXPORTS_H
