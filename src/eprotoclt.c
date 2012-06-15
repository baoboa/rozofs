/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <memory.h> /* for memset */
#include "eproto.h"
#include "rozofs.h"

/* Default timeout can be changed using clnt_control() */
static struct timeval TIMEOUT = { 25, 0 };

void *
ep_null_1(void *argp, CLIENT *clnt)
{
	static char clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, EP_NULL,
		(xdrproc_t) xdr_void, (caddr_t) argp,
		(xdrproc_t) xdr_void, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return ((void *)&clnt_res);
}

ep_mount_ret_t *
ep_mount_1(ep_path_t *argp, CLIENT *clnt)
{
	static ep_mount_ret_t clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, EP_MOUNT,
		(xdrproc_t) xdr_ep_path_t, (caddr_t) argp,
		(xdrproc_t) xdr_ep_mount_ret_t, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

ep_status_ret_t *
ep_umount_1(uint32_t *argp, CLIENT *clnt)
{
	static ep_status_ret_t clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, EP_UMOUNT,
		(xdrproc_t) xdr_uint32_t, (caddr_t) argp,
		(xdrproc_t) xdr_ep_status_ret_t, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

ep_statfs_ret_t *
ep_statfs_1(uint32_t *argp, CLIENT *clnt)
{
	static ep_statfs_ret_t clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, EP_STATFS,
		(xdrproc_t) xdr_uint32_t, (caddr_t) argp,
		(xdrproc_t) xdr_ep_statfs_ret_t, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

ep_mattr_ret_t *
ep_lookup_1(ep_lookup_arg_t *argp, CLIENT *clnt)
{
	static ep_mattr_ret_t clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, EP_LOOKUP,
		(xdrproc_t) xdr_ep_lookup_arg_t, (caddr_t) argp,
		(xdrproc_t) xdr_ep_mattr_ret_t, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

ep_mattr_ret_t *
ep_getattr_1(ep_mfile_arg_t *argp, CLIENT *clnt)
{
	static ep_mattr_ret_t clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, EP_GETATTR,
		(xdrproc_t) xdr_ep_mfile_arg_t, (caddr_t) argp,
		(xdrproc_t) xdr_ep_mattr_ret_t, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

ep_mattr_ret_t *
ep_setattr_1(ep_setattr_arg_t *argp, CLIENT *clnt)
{
	static ep_mattr_ret_t clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, EP_SETATTR,
		(xdrproc_t) xdr_ep_setattr_arg_t, (caddr_t) argp,
		(xdrproc_t) xdr_ep_mattr_ret_t, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

ep_readlink_ret_t *
ep_readlink_1(ep_mfile_arg_t *argp, CLIENT *clnt)
{
	static ep_readlink_ret_t clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, EP_READLINK,
		(xdrproc_t) xdr_ep_mfile_arg_t, (caddr_t) argp,
		(xdrproc_t) xdr_ep_readlink_ret_t, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

ep_mattr_ret_t *
ep_mknod_1(ep_mknod_arg_t *argp, CLIENT *clnt)
{
	static ep_mattr_ret_t clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, EP_MKNOD,
		(xdrproc_t) xdr_ep_mknod_arg_t, (caddr_t) argp,
		(xdrproc_t) xdr_ep_mattr_ret_t, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

ep_mattr_ret_t *
ep_mkdir_1(ep_mkdir_arg_t *argp, CLIENT *clnt)
{
	static ep_mattr_ret_t clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, EP_MKDIR,
		(xdrproc_t) xdr_ep_mkdir_arg_t, (caddr_t) argp,
		(xdrproc_t) xdr_ep_mattr_ret_t, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

ep_fid_ret_t *
ep_unlink_1(ep_unlink_arg_t *argp, CLIENT *clnt)
{
	static ep_fid_ret_t clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, EP_UNLINK,
		(xdrproc_t) xdr_ep_unlink_arg_t, (caddr_t) argp,
		(xdrproc_t) xdr_ep_fid_ret_t, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

ep_fid_ret_t *
ep_rmdir_1(ep_rmdir_arg_t *argp, CLIENT *clnt)
{
	static ep_fid_ret_t clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, EP_RMDIR,
		(xdrproc_t) xdr_ep_rmdir_arg_t, (caddr_t) argp,
		(xdrproc_t) xdr_ep_fid_ret_t, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

ep_mattr_ret_t *
ep_symlink_1(ep_symlink_arg_t *argp, CLIENT *clnt)
{
	static ep_mattr_ret_t clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, EP_SYMLINK,
		(xdrproc_t) xdr_ep_symlink_arg_t, (caddr_t) argp,
		(xdrproc_t) xdr_ep_mattr_ret_t, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

ep_fid_ret_t *
ep_rename_1(ep_rename_arg_t *argp, CLIENT *clnt)
{
	static ep_fid_ret_t clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, EP_RENAME,
		(xdrproc_t) xdr_ep_rename_arg_t, (caddr_t) argp,
		(xdrproc_t) xdr_ep_fid_ret_t, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

ep_readdir_ret_t *
ep_readdir_1(ep_readdir_arg_t *argp, CLIENT *clnt)
{
	static ep_readdir_ret_t clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, EP_READDIR,
		(xdrproc_t) xdr_ep_readdir_arg_t, (caddr_t) argp,
		(xdrproc_t) xdr_ep_readdir_ret_t, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

ep_io_ret_t *
ep_read_1(ep_io_arg_t *argp, CLIENT *clnt)
{
	static ep_io_ret_t clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, EP_READ,
		(xdrproc_t) xdr_ep_io_arg_t, (caddr_t) argp,
		(xdrproc_t) xdr_ep_io_ret_t, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

ep_read_block_ret_t *
ep_read_block_1(ep_read_block_arg_t *argp, CLIENT *clnt)
{
	static ep_read_block_ret_t clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, EP_READ_BLOCK,
		(xdrproc_t) xdr_ep_read_block_arg_t, (caddr_t) argp,
		(xdrproc_t) xdr_ep_read_block_ret_t, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

ep_io_ret_t *
ep_write_1(ep_io_arg_t *argp, CLIENT *clnt)
{
	static ep_io_ret_t clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, EP_WRITE,
		(xdrproc_t) xdr_ep_io_arg_t, (caddr_t) argp,
		(xdrproc_t) xdr_ep_io_ret_t, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

ep_status_ret_t *
ep_write_block_1(ep_write_block_arg_t *argp, CLIENT *clnt)
{
	static ep_status_ret_t clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, EP_WRITE_BLOCK,
		(xdrproc_t) xdr_ep_write_block_arg_t, (caddr_t) argp,
		(xdrproc_t) xdr_ep_status_ret_t, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

ep_status_ret_t *
ep_open_1(ep_mfile_arg_t *argp, CLIENT *clnt)
{
	static ep_status_ret_t clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, EP_OPEN,
		(xdrproc_t) xdr_ep_mfile_arg_t, (caddr_t) argp,
		(xdrproc_t) xdr_ep_status_ret_t, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

ep_status_ret_t *
ep_close_1(ep_mfile_arg_t *argp, CLIENT *clnt)
{
	static ep_status_ret_t clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, EP_CLOSE,
		(xdrproc_t) xdr_ep_mfile_arg_t, (caddr_t) argp,
		(xdrproc_t) xdr_ep_status_ret_t, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}

ep_mattr_ret_t *
ep_link_1(ep_link_arg_t *argp, CLIENT *clnt)
{
	static ep_mattr_ret_t clnt_res;

	memset((char *)&clnt_res, 0, sizeof(clnt_res));
	if (clnt_call (clnt, EP_LINK,
		(xdrproc_t) xdr_ep_link_arg_t, (caddr_t) argp,
		(xdrproc_t) xdr_ep_mattr_ret_t, (caddr_t) &clnt_res,
		TIMEOUT) != RPC_SUCCESS) {
		return (NULL);
	}
	return (&clnt_res);
}
