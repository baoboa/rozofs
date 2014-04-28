/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "epproto.h"
#include <rozofs/rozofs.h>

bool_t
xdr_epp_status_t (XDR *xdrs, epp_status_t *objp)
{
	//register int32_t *buf;

	 if (!xdr_enum (xdrs, (enum_t *) objp))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_epp_status_ret_t (XDR *xdrs, epp_status_ret_t *objp)
{
	//register int32_t *buf;

	 if (!xdr_epp_status_t (xdrs, &objp->status))
		 return FALSE;
	switch (objp->status) {
	case EPP_FAILURE:
		 if (!xdr_int (xdrs, &objp->epp_status_ret_t_u.error))
			 return FALSE;
		break;
	default:
		break;
	}
	return TRUE;
}

bool_t
xdr_epp_estat_t (XDR *xdrs, epp_estat_t *objp)
{
	//register int32_t *buf;

	 if (!xdr_uint32_t (xdrs, &objp->eid))
		 return FALSE;
	 if (!xdr_uint32_t (xdrs, &objp->vid))
		 return FALSE;
	 if (!xdr_uint32_t (xdrs, &objp->bsize))
		 return FALSE;
	 if (!xdr_uint64_t (xdrs, &objp->blocks))
		 return FALSE;
	 if (!xdr_uint64_t (xdrs, &objp->bfree))
		 return FALSE;
	 if (!xdr_uint64_t (xdrs, &objp->files))
		 return FALSE;
	 if (!xdr_uint64_t (xdrs, &objp->ffree))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_epp_sstat_t (XDR *xdrs, epp_sstat_t *objp)
{
	//register int32_t *buf;

	 if (!xdr_uint16_t (xdrs, &objp->cid))
		 return FALSE;
	 if (!xdr_uint16_t (xdrs, &objp->sid))
		 return FALSE;
	 if (!xdr_uint8_t (xdrs, &objp->status))
		 return FALSE;
	 if (!xdr_uint64_t (xdrs, &objp->size))
		 return FALSE;
	 if (!xdr_uint64_t (xdrs, &objp->free))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_epp_vstat_t (XDR *xdrs, epp_vstat_t *objp)
{
	//register int32_t *buf;

	//int i;
	 if (!xdr_uint16_t (xdrs, &objp->vid))
		 return FALSE;
	 if (!xdr_uint16_t (xdrs, &objp->bsize))
		 return FALSE;
	 if (!xdr_uint64_t (xdrs, &objp->bfree))
		 return FALSE;
	 if (!xdr_uint64_t (xdrs, &objp->blocks))
		 return FALSE;
	 if (!xdr_uint32_t (xdrs, &objp->nb_storages))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->sstats, 2048,
		sizeof (epp_sstat_t), (xdrproc_t) xdr_epp_sstat_t))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_epp_profiler_t (XDR *xdrs, epp_profiler_t *objp)
{
	//register int32_t *buf;

	//int i;
	 if (!xdr_uint64_t (xdrs, &objp->uptime))
		 return FALSE;
	 if (!xdr_uint64_t (xdrs, &objp->now))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->vers, 20,
		sizeof (uint8_t), (xdrproc_t) xdr_uint8_t))
		 return FALSE;
	 if (!xdr_uint32_t (xdrs, &objp->nb_volumes))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->vstats, 16,
		sizeof (epp_vstat_t), (xdrproc_t) xdr_epp_vstat_t))
		 return FALSE;
	 if (!xdr_uint32_t (xdrs, &objp->nb_exports))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->estats, 2048,
		sizeof (epp_estat_t), (xdrproc_t) xdr_epp_estat_t))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_epp_profiler_ret_t (XDR *xdrs, epp_profiler_ret_t *objp)
{
	//register int32_t *buf;

	 if (!xdr_epp_status_t (xdrs, &objp->status))
		 return FALSE;
	switch (objp->status) {
	case EPP_SUCCESS:
		 if (!xdr_epp_profiler_t (xdrs, &objp->epp_profiler_ret_t_u.profiler))
			 return FALSE;
		break;
	case EPP_FAILURE:
		 if (!xdr_int (xdrs, &objp->epp_profiler_ret_t_u.error))
			 return FALSE;
		break;
	default:
		break;
	}
	return TRUE;
}
