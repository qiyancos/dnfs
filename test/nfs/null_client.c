/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "null.h"


void
nfs_3(char *host)
{
	CLIENT *clnt;
	void  *result_1;
	char *nfs_null_3_arg;

#ifndef	DEBUG
	clnt = clnt_create (host, NFS, NFS_VERS, "tcp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	result_1 = nfs_null_3((void*)&nfs_null_3_arg, clnt);
	if (result_1 == (void *) NULL) {
		clnt_perror (clnt, "call failed");
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;

	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	nfs_3 (host);
exit (0);
}
