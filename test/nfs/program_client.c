/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */
#include <string.h>
#include "program.h"

void print_nfs_func()
{
	printf("----------------------------------------------------------------------------------------------------\n");
	printf("%-16s%-16s%-16s%-16s%-16s%-16s\n", "0.null", "1.getattr", "2.setattr", "3.lookup", "4.access", "5.readlink");
	printf("%-16s%-16s%-16s%-16s%-16s%-16s\n", "6.read", "7.write", "8.create", "9.mkdir", "10.symlink", "11.mknod");
	printf("%-16s%-16s%-16s%-16s%-16s%-16s\n", "12.remove", "13.rmdir", "14.rename", "15.link", "16.readdir", "17.readdirplus");
	printf("%-16s%-16s%-16s%-16s\n", "18.fsstat", "19.fsinfo", "20.pathconf", "21.commit");
	printf("%-16s\n", "-1.exit");
	printf("----------------------------------------------------------------------------------------------------\n");
	printf("\n");
}

void print_fattr3(fattr3 *info)
{
	printf("type: %d\n", info->type);
	printf("mode: %u\n", info->mode);
	printf("nlink: %u\n", info->nlink);
	printf("uid: %u\n", info->uid);
	printf("gid: %u\n", info->gid);
	printf("size: %lu\n", info->size);
	printf("used: %lu\n", info->used);
	printf("rdev.specdata1: %u\n", info->rdev.specdata1);
	printf("rdev.specdata2: %u\n", info->rdev.specdata2);
	printf("fsid: %lu\n", info->fsid);
	printf("fileid: %lu\n", info->fileid);
	printf("atime.seconds: %u\n", info->atime.seconds);
	printf("atime.nseconds: %u\n", info->atime.nseconds);
	printf("mtime.seconds: %u\n", info->mtime.seconds);
	printf("mtime.nseconds: %u\n", info->mtime.nseconds);
	printf("ctime.seconds: %u\n", info->ctime.seconds);
	printf("ctime.nseconds: %u\n", info->ctime.nseconds);
}

void print_post_op_attr(post_op_attr *info)
{
	printf("attributes_follow: %d\n", info->attributes_follow);
	if (info->attributes_follow)
		print_fattr3(&info->post_op_attr_u.attributes);
}

void print_post_op_fh3(post_op_fh3 *info)
{
	printf("handle_follows: %d\n", info->handle_follows);
	if (info->handle_follows)
	{
		printf("handle.data_val: %s\n", info->post_op_fh3_u.handle.data.data_val);
		printf("handle.data_len: %d\n", info->post_op_fh3_u.handle.data.data_len);
	}
}

void print_entryplus3(entryplus3 *entry)
{
	entryplus3 *node = entry;
	u_int i = 0;
	while (node != NULL)
	{
		printf("[index: %d] name: %s\n", i++, node->name);
		print_post_op_attr(&node->name_attributes);
		print_post_op_fh3(&node->name_handle);
		node = node->nextentry;
	}
}

void nfs_program_3(char *host)
{
	CLIENT *clnt;

#ifndef DEBUG
	clnt = clnt_create(host, NFS_PROGRAM, NFS_V3, "tcp");
	if (clnt == NULL)
	{
		clnt_pcreateerror(host);
		exit(1);
	}
#endif /* DEBUG */
	print_nfs_func();
	// char *func_name = (char*)malloc(sizeof(char) * 10);
	// printf("input func name: ");
	// scanf("%s", func_name);
	// char func_name[10];
	// char *func_name_ptr = func_name;
	// printf("input func name: ");
	// scanf("%s", func_name_ptr);
	int func_no;
	printf("input func no: ");
	scanf("%d", &func_no);
	// if (strcmp(func_name, "null") == 0)
	if (func_no == 0) // null
	{
		void *result_1;
		char *nfsproc3_null_3_arg;
		printf("%s\n", "arg is null");
		result_1 = nfsproc3_null_3((void *)&nfsproc3_null_3_arg, clnt);
		if (result_1 == (void *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	// else if (strcmp(func_name, "getattr") == 0)
	else if (func_no == 1) // getattr
	{
		GETATTR3res *result_2;
		// FSINFO3args nfsproc3_fsinfo_3_arg;
		char src[128];
		char *src_ptr = src;
		printf("input data: ");
		scanf("%s", src_ptr);
		// printf("sizeof: %lu\n", sizeof(data));
		// printf("strlen: %ld\n", strlen(data));
		u_int data_len = (u_int)strlen(src) + 1;
		char *data_val = (char *)malloc(sizeof(char) * data_len);
		char *dst_ptr = data_val;
		u_int i = data_len;
		while (i--)
		{
			*(dst_ptr++) = *(src_ptr++);
		}
		*(dst_ptr++) = '\0';
		GETATTR3args nfsproc3_getattr_3_arg = {{{data_len, data_val}}};
		result_2 = nfsproc3_getattr_3(&nfsproc3_getattr_3_arg, clnt);
		if (result_2 == (GETATTR3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
		else
		{
			print_fattr3(&result_2->GETATTR3res_u.resok.obj_attributes);
		}
	}
	// else if (strcmp(func_name, "setattr") == 0)
	else if (func_no == 2) // setattr
	{
		SETATTR3res *result_3;
		SETATTR3args nfsproc3_setattr_3_arg;
		result_3 = nfsproc3_setattr_3(&nfsproc3_setattr_3_arg, clnt);
		if (result_3 == (SETATTR3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	// else if (strcmp(func_name, "lookup") == 0)
	else if (func_no == 3) // lookup
	{
		LOOKUP3res *result_4;
		LOOKUP3args nfsproc3_lookup_3_arg;
		result_4 = nfsproc3_lookup_3(&nfsproc3_lookup_3_arg, clnt);
		if (result_4 == (LOOKUP3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	// else if (strcmp(func_name, "access") == 0)
	else if (func_no == 4) // access
	{
		ACCESS3res *result_5;
		ACCESS3args nfsproc3_access_3_arg;
		result_5 = nfsproc3_access_3(&nfsproc3_access_3_arg, clnt);
		if (result_5 == (ACCESS3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	// else if (strcmp(func_name, "readlink") == 0)
	else if (func_no == 5) // readlink
	{
		READLINK3res *result_6;
		READLINK3args nfsproc3_readlink_3_arg;
		result_6 = nfsproc3_readlink_3(&nfsproc3_readlink_3_arg, clnt);
		if (result_6 == (READLINK3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	// else if (strcmp(func_name, "read") == 0)
	else if (func_no == 6) // read
	{
		READ3res *result_7;
		READ3args nfsproc3_read_3_arg;
		result_7 = nfsproc3_read_3(&nfsproc3_read_3_arg, clnt);
		if (result_7 == (READ3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	// else if (strcmp(func_name, "write") == 0)
	else if (func_no == 7) // write
	{
		WRITE3res *result_8;
		WRITE3args nfsproc3_write_3_arg;
		result_8 = nfsproc3_write_3(&nfsproc3_write_3_arg, clnt);
		if (result_8 == (WRITE3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	// else if (strcmp(func_name, "create") == 0)
	else if (func_no == 8) // create
	{
		CREATE3res *result_9;
		CREATE3args nfsproc3_create_3_arg;
		result_9 = nfsproc3_create_3(&nfsproc3_create_3_arg, clnt);
		if (result_9 == (CREATE3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	// else if (strcmp(func_name, "mkdir") == 0)
	else if (func_no == 9) // mkdir
	{
		MKDIR3res *result_10;
		MKDIR3args nfsproc3_mkdir_3_arg;
		result_10 = nfsproc3_mkdir_3(&nfsproc3_mkdir_3_arg, clnt);
		if (result_10 == (MKDIR3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	// else if (strcmp(func_name, "symlink") == 0)
	else if (func_no == 10) // symlink
	{
		SYMLINK3res *result_11;
		SYMLINK3args nfsproc3_symlink_3_arg;
		result_11 = nfsproc3_symlink_3(&nfsproc3_symlink_3_arg, clnt);
		if (result_11 == (SYMLINK3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	// else if (strcmp(func_name, "mknod") == 0)
	else if (func_no == 11) // mknod
	{
		MKNOD3res *result_12;
		MKNOD3args nfsproc3_mknod_3_arg;
		result_12 = nfsproc3_mknod_3(&nfsproc3_mknod_3_arg, clnt);
		if (result_12 == (MKNOD3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	// else if (strcmp(func_name, "remove") == 0)
	else if (func_no == 12) // remove
	{
		REMOVE3res *result_13;
		REMOVE3args nfsproc3_remove_3_arg;
		result_13 = nfsproc3_remove_3(&nfsproc3_remove_3_arg, clnt);
		if (result_13 == (REMOVE3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	// else if (strcmp(func_name, "rmdir") == 0)
	else if (func_no == 13) // rmdir
	{
		RMDIR3res *result_14;
		RMDIR3args nfsproc3_rmdir_3_arg;
		result_14 = nfsproc3_rmdir_3(&nfsproc3_rmdir_3_arg, clnt);
		if (result_14 == (RMDIR3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	// else if (strcmp(func_name, "rename") == 0)
	else if (func_no == 14) // rename
	{
		RENAME3res *result_15;
		RENAME3args nfsproc3_rename_3_arg;
		result_15 = nfsproc3_rename_3(&nfsproc3_rename_3_arg, clnt);
		if (result_15 == (RENAME3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	// else if (strcmp(func_name, "link") == 0)
	else if (func_no == 15) // link
	{
		LINK3res *result_16;
		LINK3args nfsproc3_link_3_arg;
		result_16 = nfsproc3_link_3(&nfsproc3_link_3_arg, clnt);
		if (result_16 == (LINK3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	// else if (strcmp(func_name, "readdir") == 0)
	else if (func_no == 16) // readdir
	{
		READDIR3res *result_17;
		READDIR3args nfsproc3_readdir_3_arg;
		result_17 = nfsproc3_readdir_3(&nfsproc3_readdir_3_arg, clnt);
		if (result_17 == (READDIR3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	// else if (strcmp(func_name, "readdirplus") == 0)
	else if (func_no == 17) // readdirplus
	{
		READDIRPLUS3res *result_18;
		// READDIRPLUS3args nfsproc3_readdirplus_3_arg;
		char src[128];
		char *src_ptr = src;
		printf("input data: ");
		scanf("%s", src_ptr);
		// printf("sizeof: %lu\n", sizeof(data));
		// printf("strlen: %ld\n", strlen(data));
		u_int data_len = (u_int)strlen(src) + 1;
		char *data_val = (char *)malloc(sizeof(char) * data_len);
		char *dst_ptr = data_val;
		u_int i = data_len;
		while (i--)
		{
			*(dst_ptr++) = *(src_ptr++);
		}
		*(dst_ptr++) = '\0';
		READDIRPLUS3args nfsproc3_readdirplus_3_arg = {{{data_len, data_val}}, 0, 0, 0, 0};
		nfsproc3_readdirplus_3_arg.dircount = 65536;
		nfsproc3_readdirplus_3_arg.maxcount = 524288;
		result_18 = nfsproc3_readdirplus_3(&nfsproc3_readdirplus_3_arg, clnt);
		if (result_18 == (READDIRPLUS3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
		else
		{
			print_post_op_attr(&result_18->READDIRPLUS3res_u.resok.dir_attributes);
			print_entryplus3(result_18->READDIRPLUS3res_u.resok.reply.entries);
		}
		free(data_val);
	}
	// else if (strcmp(func_name, "fsstat") == 0)
	else if (func_no == 18) // fsstat
	{
		FSSTAT3res *result_19;
        char src[128];
        char *src_ptr = src;
        printf("input data: ");
        scanf("%s", src_ptr);
        // printf("sizeof: %lu\n", sizeof(data));
        // printf("strlen: %ld\n", strlen(data));
        u_int data_len = (u_int)strlen(src) + 1;
        char *data_val = (char *)malloc(sizeof(char) * data_len);
        char *dst_ptr = data_val;
        u_int i = data_len;
        while (i--)
        {
            *(dst_ptr++) = *(src_ptr++);
        }
        *(dst_ptr++) = '\0';
		FSSTAT3args nfsproc3_fsstat_3_arg= {{{data_len, data_val}}};
		result_19 = nfsproc3_fsstat_3(&nfsproc3_fsstat_3_arg, clnt);
		if (result_19 == (FSSTAT3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
        else
        {
            printf("-----response-----\n");
            printf("status: %d\n", result_19->status);
            if (result_19->status == 0)
            {
                printf("resok\n");
            }
            else
            {
                printf("resfail\n");
            }
            print_post_op_attr(&result_19->FSSTAT3res_u.resok.obj_attributes);
            printf("tbytes: %lu\n", result_19->FSSTAT3res_u.resok.tbytes);
            printf("fbytes: %lu\n", result_19->FSSTAT3res_u.resok.fbytes);
            printf("abytes: %lu\n", result_19->FSSTAT3res_u.resok.abytes);
            printf("tfiles: %lu\n", result_19->FSSTAT3res_u.resok.tfiles);
            printf("ffiles: %lu\n", result_19->FSSTAT3res_u.resok.ffiles);
            printf("afiles: %lu\n", result_19->FSSTAT3res_u.resok.afiles);
            printf("invarsec: %u\n", result_19->FSSTAT3res_u.resok.invarsec);
        }
	}
	// else if (strcmp(func_name, "fsinfo") == 0)
	else if (func_no == 19) // fsinfo
	{
		FSINFO3res *result_20;
		// FSINFO3args nfsproc3_fsinfo_3_arg;
		char src[128];
		char *src_ptr = src;
		printf("input data: ");
		scanf("%s", src_ptr);
		// printf("sizeof: %lu\n", sizeof(data));
		// printf("strlen: %ld\n", strlen(data));
		u_int data_len = (u_int)strlen(src) + 1;
		char *data_val = (char *)malloc(sizeof(char) * data_len);
		char *dst_ptr = data_val;
		u_int i = data_len;
		while (i--)
		{
			*(dst_ptr++) = *(src_ptr++);
		}
		*(dst_ptr++) = '\0';
		FSINFO3args nfsproc3_fsinfo_3_arg = {{{data_len, data_val}}};
		result_20 = nfsproc3_fsinfo_3(&nfsproc3_fsinfo_3_arg, clnt);
		if (result_20 == (FSINFO3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
		else
		{
			printf("-----response-----\n");
			printf("status: %d\n", result_20->status);
			if (result_20->status == 0)
			{
				printf("resok\n");
			}
			else
			{
				printf("resfail\n");
			}
			print_post_op_attr(&result_20->FSINFO3res_u.resok.obj_attributes);
			printf("rtmax: %u\n", result_20->FSINFO3res_u.resok.rtmax);
			printf("rtpref: %u\n", result_20->FSINFO3res_u.resok.rtpref);
			printf("rtmult: %u\n", result_20->FSINFO3res_u.resok.rtmult);
			printf("wtmax: %u\n", result_20->FSINFO3res_u.resok.wtmax);
			printf("wtpref: %u\n", result_20->FSINFO3res_u.resok.wtpref);
			printf("wtmult: %u\n", result_20->FSINFO3res_u.resok.wtmult);
			printf("dtpref: %u\n", result_20->FSINFO3res_u.resok.dtpref);
			printf("maxfilesize: %lu\n", result_20->FSINFO3res_u.resok.maxfilesize);
			printf("time_delta.seconds: %u\n", result_20->FSINFO3res_u.resok.time_delta.seconds);
			printf("time_delta.nseconds: %u\n", result_20->FSINFO3res_u.resok.time_delta.nseconds);
			printf("properties: %u\n", result_20->FSINFO3res_u.resok.properties);
		}
		free(data_val);
	}
	// else if (strcmp(func_name, "pathconf") == 0)
	else if (func_no == 20) // pathconf
	{
		PATHCONF3res *result_21;
		char src[128];
		char *src_ptr = src;
		printf("input data: ");
		scanf("%s", src_ptr);
		// printf("sizeof: %lu\n", sizeof(data));
		// printf("strlen: %ld\n", strlen(data));
		u_int data_len = (u_int)strlen(src) + 1;
		char *data_val = (char *)malloc(sizeof(char) * data_len);
		char *dst_ptr = data_val;
		u_int i = data_len;
		while (i--)
		{
			*(dst_ptr++) = *(src_ptr++);
		}
		*(dst_ptr++) = '\0';
		PATHCONF3args nfsproc3_pathconf_3_arg = {{{data_len, data_val}}};
		result_21 = nfsproc3_pathconf_3(&nfsproc3_pathconf_3_arg, clnt);
		if (result_21 == (PATHCONF3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
		else
		{
			printf("-----response-----\n");
			printf("status: %d\n", result_21->status);
			if (result_21->status == 0)
			{
				printf("resok\n");
			}
			else
			{
				printf("resfail\n");
			}
			print_post_op_attr(&result_21->PATHCONF3res_u.resok.obj_attributes);
			printf("linkmax: %d\n", result_21->PATHCONF3res_u.resok.linkmax);
			printf("name_max: %d\n", result_21->PATHCONF3res_u.resok.name_max);
			printf("no_trunc: %d\n", result_21->PATHCONF3res_u.resok.no_trunc);
			printf("chown_restricted: %d\n", result_21->PATHCONF3res_u.resok.chown_restricted);
			printf("case_insensitive: %d\n", result_21->PATHCONF3res_u.resok.case_insensitive);
			printf("case_preserving: %d\n", result_21->PATHCONF3res_u.resok.case_preserving);
		}
	}
	// else if (strcmp(func_name, "commit") == 0)
	else if (func_no == 21) // commit
	{
		COMMIT3res *result_22;
		COMMIT3args nfsproc3_commit_3_arg;
		result_22 = nfsproc3_commit_3(&nfsproc3_commit_3_arg, clnt);
		if (result_22 == (COMMIT3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	else
	{
		// printf("unknown func_name: %s\n", func_name);
		printf("unknown func_no\n");
	}
	printf("\n");

#ifndef DEBUG
	clnt_destroy(clnt);
#endif /* DEBUG */
}

int main(int argc, char *argv[])
{
	char *host;

	if (argc < 2)
	{
		// printf("usage: %s server_host func_name\n", argv[0]);
		// exit(1);
		host = "127.0.0.1";
		printf("using default host: %s\n", host);
	}
	else
	{
		host = argv[1];
	}
	nfs_program_3(host);
	exit(0);
}
