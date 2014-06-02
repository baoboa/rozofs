/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "eproto.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif
#include <rozofs/rozofs.h>

void
export_program_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union {
		ep_path_t ep_mount_1_arg;
		uint32_t ep_umount_1_arg;
		uint32_t ep_statfs_1_arg;
		epgw_lookup_arg_t ep_lookup_1_arg;
		epgw_mfile_arg_t ep_getattr_1_arg;
		epgw_setattr_arg_t ep_setattr_1_arg;
		epgw_mfile_arg_t ep_readlink_1_arg;
		epgw_mknod_arg_t ep_mknod_1_arg;
		epgw_mkdir_arg_t ep_mkdir_1_arg;
		epgw_unlink_arg_t ep_unlink_1_arg;
		epgw_rmdir_arg_t ep_rmdir_1_arg;
		epgw_symlink_arg_t ep_symlink_1_arg;
		epgw_rename_arg_t ep_rename_1_arg;
		epgw_readdir_arg_t ep_readdir_1_arg;
		epgw_io_arg_t ep_read_block_1_arg;
		epgw_write_block_arg_t ep_write_block_1_arg;
		epgw_link_arg_t ep_link_1_arg;
		epgw_setxattr_arg_t ep_setxattr_1_arg;
		epgw_getxattr_arg_t ep_getxattr_1_arg;
		epgw_removexattr_arg_t ep_removexattr_1_arg;
		epgw_listxattr_arg_t ep_listxattr_1_arg;
		uint16_t ep_list_cluster_1_arg;
		ep_path_t ep_conf_storage_1_arg;
		ep_gateway_t ep_poll_conf_1_arg;
		ep_path_t ep_conf_expgw_1_arg;
		epgw_lock_arg_t ep_set_file_lock_1_arg;
		epgw_lock_arg_t ep_get_file_lock_1_arg;
		epgw_lock_arg_t ep_clear_owner_file_lock_1_arg;
		epgw_lock_arg_t ep_clear_client_file_lock_1_arg;
		epgw_lock_arg_t ep_poll_file_lock_1_arg;
	} argument;
	char *result;
	xdrproc_t _xdr_argument, _xdr_result;
	char *(*local)(char *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case EP_NULL:
		_xdr_argument = (xdrproc_t) xdr_void;
		_xdr_result = (xdrproc_t) xdr_void;
		local = (char *(*)(char *, struct svc_req *)) ep_null_1_svc;
		break;

	case EP_MOUNT:
		_xdr_argument = (xdrproc_t) xdr_ep_path_t;
		_xdr_result = (xdrproc_t) xdr_epgw_mount_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_mount_1_svc;
		break;

	case EP_UMOUNT:
		_xdr_argument = (xdrproc_t) xdr_uint32_t;
		_xdr_result = (xdrproc_t) xdr_epgw_status_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_umount_1_svc;
		break;

	case EP_STATFS:
		_xdr_argument = (xdrproc_t) xdr_uint32_t;
		_xdr_result = (xdrproc_t) xdr_epgw_statfs_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_statfs_1_svc;
		break;

	case EP_LOOKUP:
		_xdr_argument = (xdrproc_t) xdr_epgw_lookup_arg_t;
		_xdr_result = (xdrproc_t) xdr_epgw_mattr_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_lookup_1_svc;
		break;

	case EP_GETATTR:
		_xdr_argument = (xdrproc_t) xdr_epgw_mfile_arg_t;
		_xdr_result = (xdrproc_t) xdr_epgw_mattr_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_getattr_1_svc;
		break;

	case EP_SETATTR:
		_xdr_argument = (xdrproc_t) xdr_epgw_setattr_arg_t;
		_xdr_result = (xdrproc_t) xdr_epgw_mattr_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_setattr_1_svc;
		break;

	case EP_READLINK:
		_xdr_argument = (xdrproc_t) xdr_epgw_mfile_arg_t;
		_xdr_result = (xdrproc_t) xdr_epgw_readlink_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_readlink_1_svc;
		break;

	case EP_MKNOD:
		_xdr_argument = (xdrproc_t) xdr_epgw_mknod_arg_t;
		_xdr_result = (xdrproc_t) xdr_epgw_mattr_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_mknod_1_svc;
		break;

	case EP_MKDIR:
		_xdr_argument = (xdrproc_t) xdr_epgw_mkdir_arg_t;
		_xdr_result = (xdrproc_t) xdr_epgw_mattr_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_mkdir_1_svc;
		break;

	case EP_UNLINK:
		_xdr_argument = (xdrproc_t) xdr_epgw_unlink_arg_t;
		_xdr_result = (xdrproc_t) xdr_epgw_fid_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_unlink_1_svc;
		break;

	case EP_RMDIR:
		_xdr_argument = (xdrproc_t) xdr_epgw_rmdir_arg_t;
		_xdr_result = (xdrproc_t) xdr_epgw_fid_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_rmdir_1_svc;
		break;

	case EP_SYMLINK:
		_xdr_argument = (xdrproc_t) xdr_epgw_symlink_arg_t;
		_xdr_result = (xdrproc_t) xdr_epgw_mattr_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_symlink_1_svc;
		break;

	case EP_RENAME:
		_xdr_argument = (xdrproc_t) xdr_epgw_rename_arg_t;
		_xdr_result = (xdrproc_t) xdr_epgw_rename_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_rename_1_svc;
		break;

	case EP_READDIR:
		_xdr_argument = (xdrproc_t) xdr_epgw_readdir_arg_t;
		_xdr_result = (xdrproc_t) xdr_epgw_readdir_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_readdir_1_svc;
		break;

	case EP_READ_BLOCK:
		_xdr_argument = (xdrproc_t) xdr_epgw_io_arg_t;
		_xdr_result = (xdrproc_t) xdr_epgw_read_block_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_read_block_1_svc;
		break;

	case EP_WRITE_BLOCK:
		_xdr_argument = (xdrproc_t) xdr_epgw_write_block_arg_t;
		_xdr_result = (xdrproc_t) xdr_epgw_mattr_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_write_block_1_svc;
		break;

	case EP_LINK:
		_xdr_argument = (xdrproc_t) xdr_epgw_link_arg_t;
		_xdr_result = (xdrproc_t) xdr_epgw_mattr_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_link_1_svc;
		break;

	case EP_SETXATTR:
		_xdr_argument = (xdrproc_t) xdr_epgw_setxattr_arg_t;
		_xdr_result = (xdrproc_t) xdr_epgw_status_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_setxattr_1_svc;
		break;

	case EP_GETXATTR:
		_xdr_argument = (xdrproc_t) xdr_epgw_getxattr_arg_t;
		_xdr_result = (xdrproc_t) xdr_epgw_getxattr_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_getxattr_1_svc;
		break;

	case EP_REMOVEXATTR:
		_xdr_argument = (xdrproc_t) xdr_epgw_removexattr_arg_t;
		_xdr_result = (xdrproc_t) xdr_epgw_status_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_removexattr_1_svc;
		break;

	case EP_LISTXATTR:
		_xdr_argument = (xdrproc_t) xdr_epgw_listxattr_arg_t;
		_xdr_result = (xdrproc_t) xdr_epgw_listxattr_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_listxattr_1_svc;
		break;

	case EP_LIST_CLUSTER:
		_xdr_argument = (xdrproc_t) xdr_uint16_t;
		_xdr_result = (xdrproc_t) xdr_epgw_cluster_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_list_cluster_1_svc;
		break;

	case EP_CONF_STORAGE:
		_xdr_argument = (xdrproc_t) xdr_ep_path_t;
		_xdr_result = (xdrproc_t) xdr_epgw_conf_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_conf_storage_1_svc;
		break;

	case EP_POLL_CONF:
		_xdr_argument = (xdrproc_t) xdr_ep_gateway_t;
		_xdr_result = (xdrproc_t) xdr_epgw_status_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_poll_conf_1_svc;
		break;

	case EP_CONF_EXPGW:
		_xdr_argument = (xdrproc_t) xdr_ep_path_t;
		_xdr_result = (xdrproc_t) xdr_ep_gw_gateway_configuration_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_conf_expgw_1_svc;
		break;

	case EP_SET_FILE_LOCK:
		_xdr_argument = (xdrproc_t) xdr_epgw_lock_arg_t;
		_xdr_result = (xdrproc_t) xdr_epgw_lock_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_set_file_lock_1_svc;
		break;

	case EP_GET_FILE_LOCK:
		_xdr_argument = (xdrproc_t) xdr_epgw_lock_arg_t;
		_xdr_result = (xdrproc_t) xdr_epgw_lock_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_get_file_lock_1_svc;
		break;

	case EP_CLEAR_OWNER_FILE_LOCK:
		_xdr_argument = (xdrproc_t) xdr_epgw_lock_arg_t;
		_xdr_result = (xdrproc_t) xdr_epgw_status_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_clear_owner_file_lock_1_svc;
		break;

	case EP_CLEAR_CLIENT_FILE_LOCK:
		_xdr_argument = (xdrproc_t) xdr_epgw_lock_arg_t;
		_xdr_result = (xdrproc_t) xdr_epgw_status_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_clear_client_file_lock_1_svc;
		break;

	case EP_POLL_FILE_LOCK:
		_xdr_argument = (xdrproc_t) xdr_epgw_lock_arg_t;
		_xdr_result = (xdrproc_t) xdr_epgw_status_ret_t;
		local = (char *(*)(char *, struct svc_req *)) ep_poll_file_lock_1_svc;
		break;

	default:
		svcerr_noproc (transp);
		return;
	}
	memset ((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		svcerr_decode (transp);
		return;
	}
	result = (*local)((char *)&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, (xdrproc_t) _xdr_result, result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		exit (1);
	}
	return;
}
