/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "storcli_proto.h"
#include <rozofs/rozofs.h>

bool_t
xdr_storcli_uuid_t (XDR *xdrs, storcli_uuid_t objp)
{
	//register int32_t *buf;

	 if (!xdr_vector (xdrs, (char *)objp, ROZOFS_UUID_SIZE,
		sizeof (u_char), (xdrproc_t) xdr_u_char))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_storcli_status_t (XDR *xdrs, storcli_status_t *objp)
{
	//register int32_t *buf;

	 if (!xdr_enum (xdrs, (enum_t *) objp))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_storcli_status_ret_t (XDR *xdrs, storcli_status_ret_t *objp)
{
	//register int32_t *buf;

	 if (!xdr_storcli_status_t (xdrs, &objp->status))
		 return FALSE;
	switch (objp->status) {
	case STORCLI_FAILURE:
		 if (!xdr_int (xdrs, &objp->storcli_status_ret_t_u.error))
			 return FALSE;
		break;
	default:
		break;
	}
	return TRUE;
}

bool_t
xdr_storcli_write_arg_t (XDR *xdrs, storcli_write_arg_t *objp)
{
	//register int32_t *buf;

	//int i;
	 if (!xdr_uint16_t (xdrs, &objp->cid))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->sid))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->empty_file))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->layout))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->bsize))
		 return FALSE;
	 if (!xdr_uint32_t (xdrs, &objp->padding))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->dist_set, ROZOFS_SAFE_MAX,
		sizeof (uint8_t), (xdrproc_t) xdr_uint8_t))
		 return FALSE;
	 if (!xdr_storcli_uuid_t (xdrs, objp->fid))
		 return FALSE;
	 if (!xdr_uint64_t (xdrs, &objp->off))
		 return FALSE;
	 if (!xdr_bytes (xdrs, (char **)&objp->data.data_val, (u_int *) &objp->data.data_len, ~0))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_storcli_write_arg_no_data_t (XDR *xdrs, storcli_write_arg_no_data_t *objp)
{
	//register int32_t *buf;

	//int i;
	 if (!xdr_uint16_t (xdrs, &objp->cid))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->sid))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->empty_file))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->layout))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->bsize))
		 return FALSE;
	 if (!xdr_uint32_t (xdrs, &objp->padding))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->dist_set, ROZOFS_SAFE_MAX,
		sizeof (uint8_t), (xdrproc_t) xdr_uint8_t))
		 return FALSE;
	 if (!xdr_storcli_uuid_t (xdrs, objp->fid))
		 return FALSE;
	 if (!xdr_uint64_t (xdrs, &objp->off))
		 return FALSE;
	 if (!xdr_uint32_t (xdrs, &objp->len))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_storcli_read_arg_t (XDR *xdrs, storcli_read_arg_t *objp)
{
	//register int32_t *buf;

	//int i;
	 if (!xdr_uint16_t (xdrs, &objp->cid))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->sid))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->layout))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->spare))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->bsize))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->dist_set, ROZOFS_SAFE_MAX,
		sizeof (uint8_t), (xdrproc_t) xdr_uint8_t))
		 return FALSE;
	 if (!xdr_storcli_uuid_t (xdrs, objp->fid))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->proj_id))
		 return FALSE;
	 if (!xdr_uint64_t (xdrs, &objp->bid))
		 return FALSE;
	 if (!xdr_uint32_t (xdrs, &objp->nb_proj))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_storcli_truncate_arg_t (XDR *xdrs, storcli_truncate_arg_t *objp)
{
	//register int32_t *buf;

	//int i;
	 if (!xdr_uint16_t (xdrs, &objp->cid))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->sid))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->layout))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->spare))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->bsize))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->dist_set, ROZOFS_SAFE_MAX,
		sizeof (uint8_t), (xdrproc_t) xdr_uint8_t))
		 return FALSE;
	 if (!xdr_storcli_uuid_t (xdrs, objp->fid))
		 return FALSE;
	 if (!xdr_uint16_t (xdrs, &objp->last_seg))
		 return FALSE;
	 if (!xdr_uint64_t (xdrs, &objp->bid))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_storcli_read_no_data_ret_t (XDR *xdrs, storcli_read_no_data_ret_t *objp)
{
	//register int32_t *buf;

	 if (!xdr_uint32_t (xdrs, &objp->alignment))
		 return FALSE;
	 if (!xdr_uint32_t (xdrs, &objp->len))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_storcli_read_ret_no_data_t (XDR *xdrs, storcli_read_ret_no_data_t *objp)
{
	//register int32_t *buf;

	 if (!xdr_storcli_status_t (xdrs, &objp->status))
		 return FALSE;
	switch (objp->status) {
	case STORCLI_SUCCESS:
		 if (!xdr_storcli_read_no_data_ret_t (xdrs, &objp->storcli_read_ret_no_data_t_u.len))
			 return FALSE;
		break;
	case STORCLI_FAILURE:
		 if (!xdr_int (xdrs, &objp->storcli_read_ret_no_data_t_u.error))
			 return FALSE;
		break;
	default:
		break;
	}
	return TRUE;
}

bool_t
xdr_storcli_read_data_ret_t (XDR *xdrs, storcli_read_data_ret_t *objp)
{
	//register int32_t *buf;

	 if (!xdr_uint32_t (xdrs, &objp->alignment))
		 return FALSE;
	 if (!xdr_bytes (xdrs, (char **)&objp->dara.dara_val, (u_int *) &objp->dara.dara_len, ~0))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_storcli_read_ret_t (XDR *xdrs, storcli_read_ret_t *objp)
{
	//register int32_t *buf;

	 if (!xdr_storcli_status_t (xdrs, &objp->status))
		 return FALSE;
	switch (objp->status) {
	case STORCLI_SUCCESS:
		 if (!xdr_storcli_read_data_ret_t (xdrs, &objp->storcli_read_ret_t_u.data))
			 return FALSE;
		break;
	case STORCLI_FAILURE:
		 if (!xdr_int (xdrs, &objp->storcli_read_ret_t_u.error))
			 return FALSE;
		break;
	default:
		break;
	}
	return TRUE;
}
