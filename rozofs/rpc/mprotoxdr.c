/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "mproto.h"
#include <rozofs/rozofs.h>

bool_t
xdr_mp_uuid_t (XDR *xdrs, mp_uuid_t objp)
{
	//register int32_t *buf;

	 if (!xdr_vector (xdrs, (char *)objp, ROZOFS_UUID_SIZE_NET,
		sizeof (uint32_t), (xdrproc_t) xdr_uint32_t))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_mp_status_t (XDR *xdrs, mp_status_t *objp)
{
	//register int32_t *buf;

	 if (!xdr_enum (xdrs, (enum_t *) objp))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_mp_status_ret_t (XDR *xdrs, mp_status_ret_t *objp)
{
	//register int32_t *buf;

	 if (!xdr_mp_status_t (xdrs, &objp->status))
		 return FALSE;
	switch (objp->status) {
	case MP_FAILURE:
		 if (!xdr_int (xdrs, &objp->mp_status_ret_t_u.error))
			 return FALSE;
		break;
	default:
		break;
	}
	return TRUE;
}

bool_t
xdr_mp_remove_arg_t (XDR *xdrs, mp_remove_arg_t *objp)
{
	//register int32_t *buf;

	//int i;
	 if (!xdr_uint16_t (xdrs, &objp->cid))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->sid))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->layout))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->dist_set, ROZOFS_SAFE_MAX_NET,
		sizeof (uint32_t), (xdrproc_t) xdr_uint32_t))
		 return FALSE;
	 if (!xdr_mp_uuid_t (xdrs, objp->fid))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_mp_stat_arg_t (XDR *xdrs, mp_stat_arg_t *objp)
{
	//register int32_t *buf;

	 if (!xdr_uint16_t (xdrs, &objp->cid))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->sid))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_mp_sstat_t (XDR *xdrs, mp_sstat_t *objp)
{
	//register int32_t *buf;

	 if (!xdr_uint64_t (xdrs, &objp->size))
		 return FALSE;
	 if (!xdr_uint64_t (xdrs, &objp->free))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_mp_stat_ret_t (XDR *xdrs, mp_stat_ret_t *objp)
{
	//register int32_t *buf;

	 if (!xdr_mp_status_t (xdrs, &objp->status))
		 return FALSE;
	switch (objp->status) {
	case MP_SUCCESS:
		 if (!xdr_mp_sstat_t (xdrs, &objp->mp_stat_ret_t_u.sstat))
			 return FALSE;
		break;
	case MP_FAILURE:
		 if (!xdr_int (xdrs, &objp->mp_stat_ret_t_u.error))
			 return FALSE;
		break;
	default:
		break;
	}
	return TRUE;
}

bool_t
xdr_mp_io_address_t (XDR *xdrs, mp_io_address_t *objp)
{
	//register int32_t *buf;

	 if (!xdr_uint32_t (xdrs, &objp->ipv4))
		 return FALSE;
	 if (!xdr_uint32_t (xdrs, &objp->port))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_mp_ports_ret_t (XDR *xdrs, mp_ports_ret_t *objp)
{
	//register int32_t *buf;

	 if (!xdr_mp_status_t (xdrs, &objp->status))
		 return FALSE;
	switch (objp->status) {
	case MP_SUCCESS:
		 if (!xdr_vector (xdrs, (char *)objp->mp_ports_ret_t_u.io_addr, STORAGE_NODE_PORTS_MAX,
			sizeof (mp_io_address_t), (xdrproc_t) xdr_mp_io_address_t))
			 return FALSE;
		break;
	case MP_FAILURE:
		 if (!xdr_int (xdrs, &objp->mp_ports_ret_t_u.error))
			 return FALSE;
		break;
	default:
		break;
	}
	return TRUE;
}

bool_t
xdr_mp_children_t (XDR *xdrs, mp_children_t *objp)
{
	//register int32_t *buf;

	 if (!xdr_pointer (xdrs, (char **)objp, sizeof (struct mp_child_t), (xdrproc_t) xdr_mp_child_t))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_mp_child_t (XDR *xdrs, mp_child_t *objp)
{
	//register int32_t *buf;

	//int i;
	 if (!xdr_mp_uuid_t (xdrs, objp->fid))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->layout))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->dist_set, ROZOFS_SAFE_MAX,
		sizeof (uint8_t), (xdrproc_t) xdr_uint8_t))
		 return FALSE;
	 if (!xdr_mp_children_t (xdrs, &objp->next))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_bins_files_list_t (XDR *xdrs, bins_files_list_t *objp)
{
	//register int32_t *buf;

	//int i;
	 if (!xdr_mp_children_t (xdrs, &objp->children))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->eof))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->device))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->layout))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->spare))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->dist_set, ROZOFS_SAFE_MAX,
		sizeof (uint8_t), (xdrproc_t) xdr_uint8_t))
		 return FALSE;
	 if (!xdr_uint64_t (xdrs, &objp->cookie))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_mp_list_bins_files_ret_t (XDR *xdrs, mp_list_bins_files_ret_t *objp)
{
	//register int32_t *buf;

	 if (!xdr_mp_status_t (xdrs, &objp->status))
		 return FALSE;
	switch (objp->status) {
	case MP_SUCCESS:
		 if (!xdr_bins_files_list_t (xdrs, &objp->mp_list_bins_files_ret_t_u.reply))
			 return FALSE;
		break;
	case MP_FAILURE:
		 if (!xdr_int (xdrs, &objp->mp_list_bins_files_ret_t_u.error))
			 return FALSE;
		break;
	default:
		break;
	}
	return TRUE;
}

bool_t
xdr_mp_list_bins_files_arg_t (XDR *xdrs, mp_list_bins_files_arg_t *objp)
{
	//register int32_t *buf;

	//int i;
	 if (!xdr_uint16_t (xdrs, &objp->cid))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->sid))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->rebuild_sid))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->device))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->layout))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->dist_set, ROZOFS_SAFE_MAX,
		sizeof (uint8_t), (xdrproc_t) xdr_uint8_t))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->spare))
		 return FALSE;
	 if (!xdr_uint64_t (xdrs, &objp->cookie))
		 return FALSE;
	return TRUE;
}
