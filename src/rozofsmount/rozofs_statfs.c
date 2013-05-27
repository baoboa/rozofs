/*
  Copyright (c) 2010 Fizians SAS. <http://www.fizians.com>
  This file is part of Rozofs.

  Rozofs is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published
  by the Free Software Foundation, version 2.

  Rozofs is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see
  <http://www.gnu.org/licenses/>.
 */

/* need for crypt */
#define _XOPEN_SOURCE 500
#define FUSE_USE_VERSION 26

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <assert.h>
#include <netinet/tcp.h>

#include <fuse/fuse_lowlevel.h>
#include <fuse/fuse_opt.h>

#include <rozofs/rozofs.h>
#include <rozofs/common/list.h>
#include <rozofs/common/log.h>
#include <rozofs/common/htable.h>
#include <rozofs/common/xmalloc.h>
#include <rozofs/common/profile.h>
#include <rozofs/rpc/sclient.h>
#include <rozofs/rpc/mclient.h>
#include <rozofs/rpc/mpproto.h>
#include <rozofs/rpc/eproto.h>
#include "config.h"
#include "file.h"
#include "rozofs_fuse.h"
#include "rozofs_fuse_api.h"
#include "rozofsmount.h"
#include <rozofs/core/rozofs_tx_common.h>
#include <rozofs/core/rozofs_tx_api.h>
#include <rozofs/rozofs_timer_conf.h>

DECLARE_PROFILING(mpp_profiler_t);


/**
* Get file system statistics
*
* Valid replies:
*   fuse_reply_statfs
*   fuse_reply_err
*
* @param req request handle
* @param ino the inode number, zero means "undefined"
*/
 void rozofs_ll_statfs_cbk(void *this,void *param) ;
 
 
void rozofs_ll_statfs_nb(fuse_req_t req, fuse_ino_t ino) {
    (void) ino;
    int    ret;        
    void *buffer_p = NULL;
    /*
    ** allocate a context for saving the fuse parameters
    */
    buffer_p = rozofs_fuse_alloc_saved_context();
    if (buffer_p == NULL)
    {
      severe("out of fuse saved context");
      errno = ENOMEM;
      goto error;
    }
    SAVE_FUSE_PARAM(buffer_p,req);
    SAVE_FUSE_PARAM(buffer_p,ino);

    START_PROFILING_NB(buffer_p,rozofs_ll_statfs);
    /*
    ** now initiates the transaction towards the remote end
    */
    ret = rozofs_export_send_common(&exportclt,ROZOFS_TMR_GET(TMR_EXPORT_PROGRAM),EXPORT_PROGRAM, EXPORT_VERSION,
                              EP_STATFS,(xdrproc_t) xdr_uint32_t,(void *)&exportclt.eid,
                              rozofs_ll_statfs_cbk,buffer_p); 
    if (ret < 0) goto error;
    
    /*
    ** no error just waiting for the answer
    */
    return;
error:
    fuse_reply_err(req, errno);
    /*
    ** release the buffer if has been allocated
    */
    STOP_PROFILING_NB(buffer_p,rozofs_ll_statfs);
    if (buffer_p != NULL) rozofs_fuse_release_saved_context(buffer_p);
    return;
}


/**
*  Call back function call upon a success rpc, timeout or any other rpc failure
*
 @param this : pointer to the transaction context
 @param param: pointer to the associated rozofs_fuse_context
 
 @return none
 */
 void rozofs_ll_statfs_cbk(void *this,void *param) 
{
   fuse_req_t req; 
   struct statvfs st;
   estat_t estat;
   int status;
   uint8_t  *payload;
   void     *recv_buf = NULL;   
   XDR       xdrs;    
   int      bufsize;
   epgw_statfs_ret_t ret;
   struct rpc_msg  rpc_reply;
   xdrproc_t decode_proc = (xdrproc_t)xdr_epgw_statfs_ret_t;

   
   rpc_reply.acpted_rply.ar_results.proc = NULL;
   RESTORE_FUSE_PARAM(param,req);
    /*
    ** get the pointer to the transaction context:
    ** it is required to get the information related to the receive buffer
    */
    rozofs_tx_ctx_t      *rozofs_tx_ctx_p = (rozofs_tx_ctx_t*)this;     
    /*    
    ** get the status of the transaction -> 0 OK, -1 error (need to get errno for source cause
    */
    status = rozofs_tx_get_status(this);
    if (status < 0)
    {
       /*
       ** something wrong happened
       */
       errno = rozofs_tx_get_errno(this);  
       goto error; 
    }
    /*
    ** get the pointer to the receive buffer payload
    */
    recv_buf = rozofs_tx_get_recvBuf(this);
    if (recv_buf == NULL)
    {
       /*
       ** something wrong happened
       */
       errno = EFAULT;  
       goto error;         
    }
    payload  = (uint8_t*) ruc_buf_getPayload(recv_buf);
    payload += sizeof(uint32_t); /* skip length*/ 
    /*
    ** OK now decode the received message
    */
    bufsize = (int) ruc_buf_getPayloadLen(recv_buf);
    xdrmem_create(&xdrs,(char*)payload,bufsize,XDR_DECODE);
    /*
    ** decode the rpc part
    */
    if (rozofs_xdr_replymsg(&xdrs,&rpc_reply) != TRUE)
    {
     TX_STATS(ROZOFS_TX_DECODING_ERROR);
     errno = EPROTO;
     goto error;
    }
    /*
    ** ok now call the procedure to encode the message
    */
    memset((char *)&ret, 0, sizeof(ret));
    if (decode_proc(&xdrs,&ret) == FALSE)
    {
       TX_STATS(ROZOFS_TX_DECODING_ERROR);
       errno = EPROTO;
       xdr_free((xdrproc_t) decode_proc, (char *) &ret);
       goto error;
    }   
    if (ret.status_gw.status == EP_FAILURE) {
        errno = ret.status_gw.ep_statfs_ret_t_u.error;
        xdr_free((xdrproc_t) decode_proc, (char *) &ret);    
        goto error;
    }
    memcpy(&estat, &ret.status_gw.ep_statfs_ret_t_u.stat, sizeof (ep_statfs_t));
    xdr_free((xdrproc_t) decode_proc, (char *) &ret);    
    /*
    ** end of decoding section
    */
    memset(&st, 0, sizeof (struct statvfs));
    st.f_blocks = estat.blocks; // + estat.bfree;
    st.f_bavail = st.f_bfree = estat.bfree;
    st.f_frsize = st.f_bsize = estat.bsize;
    st.f_favail = st.f_ffree = estat.ffree;
    st.f_files = estat.files;
    st.f_namemax = estat.namemax;

    fuse_reply_statfs(req, &st);
    goto out;
error:
    fuse_reply_err(req, errno);
out:
    /*
    ** release the transaction context and the fuse context
    */
    STOP_PROFILING_NB(param,rozofs_ll_statfs);
    rozofs_fuse_release_saved_context(param);
    if (rozofs_tx_ctx_p != NULL) rozofs_tx_free_from_ptr(rozofs_tx_ctx_p);    
    if (recv_buf != NULL) ruc_buf_freeBuffer(recv_buf);   
    return;
}
