/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include <memory.h> /* for memset */
#include <rozofs/rpc/gwproto.h>
#include <rozofs/rozofs.h>
#include <rozofs/core/rozofs_rpc_non_blocking_generic.h>
#include <rozofs/rpc/export_profiler.h>
#include <rozofs/rpc/epproto.h>

#define GW_NAME_LEN (ROZOFS_HOSTNAME_MAX/4)
//  uint32_t           eid[EXPGW_EID_MAX_IDX];  
//  gw_host_conf_t     gateway_host[EXPGW_EXPGW_MAX_IDX];

/* Default timeout can be changed using clnt_control() */
//static struct timeval recv_cbk,ctx_p = { 25, 0 };

extern void gw_poll_1_nblocking_cbk();
extern void gw_configuration_1_nblocking_cbk();
#if 0
int 
gw_null_1_nblocking(void *argp, int lbg_id,void *ctx_p)
{
	static char clnt_res;

	if (rozofs_rpc_non_blocking_req_send (lbg_id,GW_PROGRAM,GW_VERSION, GW_NULL,NULL,
		(xdrproc_t) xdr_void, (caddr_t) argp,
		(xdrproc_t) xdr_void, (void*) &clnt_res,sizeof(clnt_res),
		gw_null_1_nblocking_cbk,ctx_p) < 0 ) {
		return -1;
	}
	return 0;
}
int 
gw_invalidate_sections_1_nblocking(gw_invalidate_sections_t *argp, int lbg_id,void *ctx_p)
{
	static gw_status_t clnt_res;

	if (rozofs_rpc_non_blocking_req_send (lbg_id,GW_PROGRAM,GW_VERSION, GW_INVALIDATE_SECTIONS,NULL,
		(xdrproc_t) xdr_gw_invalidate_sections_t, (caddr_t) argp,
		(xdrproc_t) xdr_gw_status_t, (void*) &clnt_res,sizeof(clnt_res),
		gw_invalidate_sections_1_nblocking_cbk,ctx_p) < 0 ) {
		return -1;
	}
	return 0;
}

int 
gw_invalidate_all_1_nblocking(gw_header_t *argp, int lbg_id,void *ctx_p)
{
	static gw_status_t clnt_res;

	if (rozofs_rpc_non_blocking_req_send (lbg_id,GW_PROGRAM,GW_VERSION, GW_INVALIDATE_ALL,NULL,
		(xdrproc_t) xdr_gw_header_t, (caddr_t) argp,
		(xdrproc_t) xdr_gw_status_t, (void*) &clnt_res,sizeof(clnt_res),
		gw_invalidate_all_1_nblocking_cbk,ctx_p) < 0 ) {
		return -1;
	}
	return 0;
}
#endif
int 
gw_configuration_1_nblocking(gw_configuration_t *argp, int lbg_id,void *ctx_p)
{
	static gw_status_t clnt_res;

	if (rozofs_rpc_non_blocking_req_send (lbg_id,GW_PROGRAM,GW_VERSION, GW_CONFIGURATION,NULL,
		(xdrproc_t) xdr_gw_configuration_t, (caddr_t) argp,
		(xdrproc_t) xdr_gw_status_t, (void*) &clnt_res,sizeof(clnt_res),
		gw_configuration_1_nblocking_cbk,ctx_p) < 0 ) {
		return -1;
	}
	return 0;
}

int 
gw_poll_1_nblocking(gw_header_t *argp, int lbg_id,void *ctx_p)
{
	static gw_status_t clnt_res;
        uint64_t           val;
	
	GET_EID_PROB_COUNT(val,(argp->export_id),gw_poll);		

	if (rozofs_rpc_non_blocking_req_send (lbg_id,GW_PROGRAM,GW_VERSION, GW_POLL,&val,
		(xdrproc_t) xdr_gw_header_t, (caddr_t) argp,
		(xdrproc_t) xdr_gw_status_t, (void*) &clnt_res,sizeof(clnt_res),
		gw_poll_1_nblocking_cbk,ctx_p) < 0 ) {
		return -1;
	}
	return 0;
}

