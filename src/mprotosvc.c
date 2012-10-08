/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "../src/mproto.h"
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
#include "rozofs.h"

void
monitor_program_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union {
		uint16_t mp_stat_1_arg;
		mp_remove_arg_t mp_remove_1_arg;
	} argument;
	char *result;
	xdrproc_t _xdr_argument, _xdr_result;
	char *(*local)(char *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case MP_NULL:
		_xdr_argument = (xdrproc_t) xdr_void;
		_xdr_result = (xdrproc_t) xdr_void;
		local = (char *(*)(char *, struct svc_req *)) mp_null_1_svc;
		break;

	case MP_STAT:
		_xdr_argument = (xdrproc_t) xdr_uint16_t;
		_xdr_result = (xdrproc_t) xdr_mp_stat_ret_t;
		local = (char *(*)(char *, struct svc_req *)) mp_stat_1_svc;
		break;

	case MP_REMOVE:
		_xdr_argument = (xdrproc_t) xdr_mp_remove_arg_t;
		_xdr_result = (xdrproc_t) xdr_mp_status_ret_t;
		local = (char *(*)(char *, struct svc_req *)) mp_remove_1_svc;
		break;

	case MP_PORTS:
		_xdr_argument = (xdrproc_t) xdr_void;
		_xdr_result = (xdrproc_t) xdr_mp_ports_ret_t;
		local = (char *(*)(char *, struct svc_req *)) mp_ports_1_svc;
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
