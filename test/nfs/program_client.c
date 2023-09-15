/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "program.h"

void nfs_program_3(char *host, char *func_name)
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

	if (strcmp(func_name, "null") == 0)
	{
		void *result_1;
		char *nfsproc3_null_3_arg;
		result_1 = nfsproc3_null_3((void *)&nfsproc3_null_3_arg, clnt);
		if (result_1 == (void *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	else if (strcmp(func_name, "getattr") == 0)
	{
		GETATTR3res *result_2;
		GETATTR3args nfsproc3_getattr_3_arg;
		result_2 = nfsproc3_getattr_3(&nfsproc3_getattr_3_arg, clnt);
		if (result_2 == (GETATTR3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	else if (strcmp(func_name, "setattr") == 0)
	{
		SETATTR3res *result_3;
		SETATTR3args nfsproc3_setattr_3_arg;
		result_3 = nfsproc3_setattr_3(&nfsproc3_setattr_3_arg, clnt);
		if (result_3 == (SETATTR3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	else if (strcmp(func_name, "lookup") == 0)
	{
		LOOKUP3res *result_4;
		LOOKUP3args nfsproc3_lookup_3_arg;
		result_4 = nfsproc3_lookup_3(&nfsproc3_lookup_3_arg, clnt);
		if (result_4 == (LOOKUP3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	else if (strcmp(func_name, "access") == 0)
	{
		ACCESS3res *result_5;
		ACCESS3args nfsproc3_access_3_arg;
		result_5 = nfsproc3_access_3(&nfsproc3_access_3_arg, clnt);
		if (result_5 == (ACCESS3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	else if (strcmp(func_name, "readlink") == 0)
	{
		READLINK3res *result_6;
		READLINK3args nfsproc3_readlink_3_arg;
		result_6 = nfsproc3_readlink_3(&nfsproc3_readlink_3_arg, clnt);
		if (result_6 == (READLINK3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	else if (strcmp(func_name, "read") == 0)
	{
		READ3res *result_7;
		READ3args nfsproc3_read_3_arg;
		result_7 = nfsproc3_read_3(&nfsproc3_read_3_arg, clnt);
		if (result_7 == (READ3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	else if (strcmp(func_name, "write") == 0)
	{
		WRITE3res *result_8;
		WRITE3args nfsproc3_write_3_arg;
		result_8 = nfsproc3_write_3(&nfsproc3_write_3_arg, clnt);
		if (result_8 == (WRITE3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	else if (strcmp(func_name, "create") == 0)
	{
		CREATE3res *result_9;
		CREATE3args nfsproc3_create_3_arg;
		result_9 = nfsproc3_create_3(&nfsproc3_create_3_arg, clnt);
		if (result_9 == (CREATE3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	else if (strcmp(func_name, "mkdir") == 0)
	{
		MKDIR3res *result_10;
		MKDIR3args nfsproc3_mkdir_3_arg;
		result_10 = nfsproc3_mkdir_3(&nfsproc3_mkdir_3_arg, clnt);
		if (result_10 == (MKDIR3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	else if (strcmp(func_name, "symlink") == 0)
	{
		SYMLINK3res *result_11;
		SYMLINK3args nfsproc3_symlink_3_arg;
		result_11 = nfsproc3_symlink_3(&nfsproc3_symlink_3_arg, clnt);
		if (result_11 == (SYMLINK3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	else if (strcmp(func_name, "mknod") == 0)
	{
		MKNOD3res *result_12;
		MKNOD3args nfsproc3_mknod_3_arg;
		result_12 = nfsproc3_mknod_3(&nfsproc3_mknod_3_arg, clnt);
		if (result_12 == (MKNOD3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	else if (strcmp(func_name, "remove") == 0)
	{
		REMOVE3res *result_13;
		REMOVE3args nfsproc3_remove_3_arg;
		result_13 = nfsproc3_remove_3(&nfsproc3_remove_3_arg, clnt);
		if (result_13 == (REMOVE3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	else if (strcmp(func_name, "rmdir") == 0)
	{
		RMDIR3res *result_14;
		RMDIR3args nfsproc3_rmdir_3_arg;
		result_14 = nfsproc3_rmdir_3(&nfsproc3_rmdir_3_arg, clnt);
		if (result_14 == (RMDIR3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	else if (strcmp(func_name, "rename") == 0)
	{
		RENAME3res *result_15;
		RENAME3args nfsproc3_rename_3_arg;
		result_15 = nfsproc3_rename_3(&nfsproc3_rename_3_arg, clnt);
		if (result_15 == (RENAME3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	else if (strcmp(func_name, "link") == 0)
	{
		LINK3res *result_16;
		LINK3args nfsproc3_link_3_arg;
		result_16 = nfsproc3_link_3(&nfsproc3_link_3_arg, clnt);
		if (result_16 == (LINK3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	else if (strcmp(func_name, "readdir") == 0)
	{
		READDIR3res *result_17;
		READDIR3args nfsproc3_readdir_3_arg;
		result_17 = nfsproc3_readdir_3(&nfsproc3_readdir_3_arg, clnt);
		if (result_17 == (READDIR3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	else if (strcmp(func_name, "readdirplus") == 0)
	{
		READDIRPLUS3res *result_18;
		READDIRPLUS3args nfsproc3_readdirplus_3_arg;
		result_18 = nfsproc3_readdirplus_3(&nfsproc3_readdirplus_3_arg, clnt);
		if (result_18 == (READDIRPLUS3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	else if (strcmp(func_name, "fsstat") == 0)
	{
		FSSTAT3res *result_19;
		FSSTAT3args nfsproc3_fsstat_3_arg;
		result_19 = nfsproc3_fsstat_3(&nfsproc3_fsstat_3_arg, clnt);
		if (result_19 == (FSSTAT3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	else if (strcmp(func_name, "fsinfo") == 0)
	{
		FSINFO3res *result_20;
		FSINFO3args nfsproc3_fsinfo_3_arg;
		result_20 = nfsproc3_fsinfo_3(&nfsproc3_fsinfo_3_arg, clnt);
		if (result_20 == (FSINFO3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	else if (strcmp(func_name, "pathconf") == 0)
	{
		PATHCONF3res *result_21;
		PATHCONF3args nfsproc3_pathconf_3_arg;
		result_21 = nfsproc3_pathconf_3(&nfsproc3_pathconf_3_arg, clnt);
		if (result_21 == (PATHCONF3res *)NULL)
		{
			clnt_perror(clnt, "call failed");
		}
	}
	else if (strcmp(func_name, "commit") == 0)
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
		printf("unknown func_name: %s", func_name);
	}

#ifndef DEBUG
	clnt_destroy(clnt);
#endif /* DEBUG */
}

int main(int argc, char *argv[])
{
	char *host;
	char *func_name;

	if (argc < 3)
	{
		printf("usage: %s server_host func_name\n", argv[0]);
		exit(1);
	}
	host = argv[1];
	func_name = argv[2];
	nfs_program_3(host, func_name);
	exit(0);
}
