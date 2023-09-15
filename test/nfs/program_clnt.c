/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <memory.h> /* for memset */
#include "program.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

void *
nfsproc3_null_3(void *argp, CLIENT *clnt)
{
	static char clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, NFSPROC3_NULL,
		(xdrproc_t) xdr_void, (caddr_t) argp,
		(xdrproc_t) xdr_void, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return ((void *)&clnt_res);
}

GETATTR3res *
nfsproc3_getattr_3(GETATTR3args *argp, CLIENT *clnt)
{
	static GETATTR3res clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, NFSPROC3_GETATTR,
		(xdrproc_t) xdr_GETATTR3args, (caddr_t) argp,
		(xdrproc_t) xdr_GETATTR3res, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

SETATTR3res *
nfsproc3_setattr_3(SETATTR3args *argp, CLIENT *clnt)
{
	static SETATTR3res clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, NFSPROC3_SETATTR,
		(xdrproc_t) xdr_SETATTR3args, (caddr_t) argp,
		(xdrproc_t) xdr_SETATTR3res, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

LOOKUP3res *
nfsproc3_lookup_3(LOOKUP3args *argp, CLIENT *clnt)
{
	static LOOKUP3res clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, NFSPROC3_LOOKUP,
		(xdrproc_t) xdr_LOOKUP3args, (caddr_t) argp,
		(xdrproc_t) xdr_LOOKUP3res, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

ACCESS3res *
nfsproc3_access_3(ACCESS3args *argp, CLIENT *clnt)
{
	static ACCESS3res clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, NFSPROC3_ACCESS,
		(xdrproc_t) xdr_ACCESS3args, (caddr_t) argp,
		(xdrproc_t) xdr_ACCESS3res, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

READLINK3res *
nfsproc3_readlink_3(READLINK3args *argp, CLIENT *clnt)
{
	static READLINK3res clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, NFSPROC3_READLINK,
		(xdrproc_t) xdr_READLINK3args, (caddr_t) argp,
		(xdrproc_t) xdr_READLINK3res, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

READ3res *
nfsproc3_read_3(READ3args *argp, CLIENT *clnt)
{
	static READ3res clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, NFSPROC3_READ,
		(xdrproc_t) xdr_READ3args, (caddr_t) argp,
		(xdrproc_t) xdr_READ3res, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

WRITE3res *
nfsproc3_write_3(WRITE3args *argp, CLIENT *clnt)
{
	static WRITE3res clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, NFSPROC3_WRITE,
		(xdrproc_t) xdr_WRITE3args, (caddr_t) argp,
		(xdrproc_t) xdr_WRITE3res, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

CREATE3res *
nfsproc3_create_3(CREATE3args *argp, CLIENT *clnt)
{
	static CREATE3res clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, NFSPROC3_CREATE,
		(xdrproc_t) xdr_CREATE3args, (caddr_t) argp,
		(xdrproc_t) xdr_CREATE3res, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

MKDIR3res *
nfsproc3_mkdir_3(MKDIR3args *argp, CLIENT *clnt)
{
	static MKDIR3res clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, NFSPROC3_MKDIR,
		(xdrproc_t) xdr_MKDIR3args, (caddr_t) argp,
		(xdrproc_t) xdr_MKDIR3res, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

SYMLINK3res *
nfsproc3_symlink_3(SYMLINK3args *argp, CLIENT *clnt)
{
	static SYMLINK3res clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, NFSPROC3_SYMLINK,
		(xdrproc_t) xdr_SYMLINK3args, (caddr_t) argp,
		(xdrproc_t) xdr_SYMLINK3res, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

MKNOD3res *
nfsproc3_mknod_3(MKNOD3args *argp, CLIENT *clnt)
{
	static MKNOD3res clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, NFSPROC3_MKNOD,
		(xdrproc_t) xdr_MKNOD3args, (caddr_t) argp,
		(xdrproc_t) xdr_MKNOD3res, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

REMOVE3res *
nfsproc3_remove_3(REMOVE3args *argp, CLIENT *clnt)
{
	static REMOVE3res clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, NFSPROC3_REMOVE,
		(xdrproc_t) xdr_REMOVE3args, (caddr_t) argp,
		(xdrproc_t) xdr_REMOVE3res, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

RMDIR3res *
nfsproc3_rmdir_3(RMDIR3args *argp, CLIENT *clnt)
{
	static RMDIR3res clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, NFSPROC3_RMDIR,
		(xdrproc_t) xdr_RMDIR3args, (caddr_t) argp,
		(xdrproc_t) xdr_RMDIR3res, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

RENAME3res *
nfsproc3_rename_3(RENAME3args *argp, CLIENT *clnt)
{
	static RENAME3res clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, NFSPROC3_RENAME,
		(xdrproc_t) xdr_RENAME3args, (caddr_t) argp,
		(xdrproc_t) xdr_RENAME3res, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

LINK3res *
nfsproc3_link_3(LINK3args *argp, CLIENT *clnt)
{
	static LINK3res clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, NFSPROC3_LINK,
		(xdrproc_t) xdr_LINK3args, (caddr_t) argp,
		(xdrproc_t) xdr_LINK3res, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

READDIR3res *
nfsproc3_readdir_3(READDIR3args *argp, CLIENT *clnt)
{
	static READDIR3res clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, NFSPROC3_READDIR,
		(xdrproc_t) xdr_READDIR3args, (caddr_t) argp,
		(xdrproc_t) xdr_READDIR3res, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

READDIRPLUS3res *
nfsproc3_readdirplus_3(READDIRPLUS3args *argp, CLIENT *clnt)
{
	static READDIRPLUS3res clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, NFSPROC3_READDIRPLUS,
		(xdrproc_t) xdr_READDIRPLUS3args, (caddr_t) argp,
		(xdrproc_t) xdr_READDIRPLUS3res, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

FSSTAT3res *
nfsproc3_fsstat_3(FSSTAT3args *argp, CLIENT *clnt)
{
	static FSSTAT3res clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, NFSPROC3_FSSTAT,
		(xdrproc_t) xdr_FSSTAT3args, (caddr_t) argp,
		(xdrproc_t) xdr_FSSTAT3res, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

FSINFO3res *
nfsproc3_fsinfo_3(FSINFO3args *argp, CLIENT *clnt)
{
	static FSINFO3res clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, NFSPROC3_FSINFO,
		(xdrproc_t) xdr_FSINFO3args, (caddr_t) argp,
		(xdrproc_t) xdr_FSINFO3res, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

PATHCONF3res *
nfsproc3_pathconf_3(PATHCONF3args *argp, CLIENT *clnt)
{
	static PATHCONF3res clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, NFSPROC3_PATHCONF,
		(xdrproc_t) xdr_PATHCONF3args, (caddr_t) argp,
		(xdrproc_t) xdr_PATHCONF3res, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

COMMIT3res *
nfsproc3_commit_3(COMMIT3args *argp, CLIENT *clnt)
{
	static COMMIT3res clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, NFSPROC3_COMMIT,
		(xdrproc_t) xdr_COMMIT3args, (caddr_t) argp,
		(xdrproc_t) xdr_COMMIT3res, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}
