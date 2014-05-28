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



#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <rpc/rpc.h>
#include <rpc/pmap_clnt.h>
#include <pthread.h>

#include <rozofs/rozofs.h>
#include <rozofs/common/log.h>
#include <rozofs/common/xmalloc.h>
#include <rozofs/rpc/eproto.h>
#include <rozofs/rpc/eclient.h>
#include <rozofs/rpc/mclient.h>
#include <rozofs/core/north_lbg_api.h>
#include "rozofs_storcli_reload_storage_config.h"
#include "storcli_main.h"
#include "rozofs_storcli_lbg_cnf_supervision.h"
#include <rozofs/rpc/storcli_lbg_prototypes.h>

storcli_conf_ctx_t storcli_conf_ctx;      /**< statistics associated with exportd configuration polling and reload */


/*__________________________________________________________________________
*/
/**
*  reload of the configuration of the storage for a STORLI

   @param ret: decoded message that contains the request configuration
   
   @retval 0 on success
   @retval -1 on error
   
*/
int exportclt_reload_check_mstorage(epgw_conf_ret_t *ret,exportclt_t *exportclt_p)
{
    int node;
    int stor_len =  ret->status_gw.ep_conf_ret_t_u.export.storage_nodes.storage_nodes_len;
    ep_cnf_storage_node_t *stor_p;
    stor_p = ret->status_gw.ep_conf_ret_t_u.export.storage_nodes.storage_nodes_val;

    
    for (node = 0; node < stor_len; node++,stor_p++) 
    {
      int i = 0;
      int ret;
      list_t *iterator = NULL;
      int found = 0;
      /* Search if the node has already been created  */
      list_for_each_forward(iterator, &exportclt_p->storages) 
      {
        mstorage_t *s = list_entry(iterator, mstorage_t, list);

        if (strcmp(s->host,stor_p->host) != 0) continue;
        /*
        ** entry is found 
        ** update the cid and sid part only. changing the number of
        ** ports of the mstorage is not yet supported.
        */
        s->sids_nb = stor_p->sids_nb;
        memcpy(s->sids, stor_p->sids, sizeof (sid_t) * stor_p->sids_nb);
        memcpy(s->cids, stor_p->cids, sizeof (cid_t) * stor_p->sids_nb);
        /*
        ** update of the cid/sid<-->lbg_id association table
        */
        for (i = 0; i < s->sids_nb;i++)
        {
          rozofs_storcli_cid_table_insert(s->cids[i],s->sids[i],s->lbg_id);       
        }
        found = 1;
        break;
      }
      /*
      ** Check if the node has been found in the configuration. If it is the
      ** case, check the next node the the mount response 
      */
      if (found) continue;
      /*
      ** This is a new node, so create a new entry for it
      */
      mstorage_t *mstor = (mstorage_t *) xmalloc(sizeof (mstorage_t));
      memset(mstor, 0, sizeof (mstorage_t));
      mstor->lbg_id = -1;  /**< lbg not yet allocated */
      strcpy(mstor->host, stor_p->host);
      mstor->sids_nb = stor_p->sids_nb;
      memcpy(mstor->sids, stor_p->sids, sizeof (sid_t) * stor_p->sids_nb);
      memcpy(mstor->cids, stor_p->cids, sizeof (cid_t) *stor_p->sids_nb);

      /* Add to the list */
      list_push_back(&exportclt_p->storages, &mstor->list);
      /*
      ** Now create the load balancing group associated with that node and
      ** attempt to get its port configuration
      */        
      mclient_t mclt;
      init_rpcctl_ctx(&mclt.rpcclt);
      strcpy(mclt.host, mstor->host);
      mp_io_address_t io_address[STORAGE_NODE_PORTS_MAX];
      //memset(io_address, 0, sizeof (io_address));
      /*
      ** allocate the load balancing group for the mstorage
      */
      storcli_sup_send_lbg_create(STORCLI_LBG_CREATE,mstor);
      if (mstor->lbg_id < 0)
      {
        severe(" out of lbg contexts");
        goto fatal;        
      }
      struct timeval timeout_mproto;
      timeout_mproto.tv_sec = ROZOFS_MPROTO_TIMEOUT_SEC;
      timeout_mproto.tv_usec = 0;
      /* Initialize connection with storage (by mproto) */
      if (mclient_initialize(&mclt,timeout_mproto) != 0) 
      {
          fprintf(stderr, "Warning: failed to join storage (host: %s), %s.\n",
                  mstor->host, strerror(errno));
      } 
      else 
      {
        /* Send request to get storage TCP ports */
        if (mclient_ports(&mclt, io_address) != 0) 
        {
            fprintf(stderr,
                    "Warning: failed to get ports for storage (host: %s).\n"
                    , mstor->host);
        }
      }
      /* Initialize each TCP ports connection with this storage node
       *  (by sproto) */
      for (i = 0; i < STORAGE_NODE_PORTS_MAX; i++) 
      {
         if (io_address[i].port  != 0) 
         {
	     uint32_t ip = io_address[i].ipv4;
             sprintf(mstor->sclients[i].host, "%u.%u.%u.%u", ip>>24, (ip>>16)&0xFF,(ip>>8)&0xFF,ip&0xFF);
             mstor->sclients[i].port = io_address[i].port;
             mstor->sclients[i].status = 0;
             mstor->sclients_nb++;
         }
      }
      /*
      ** proceed with storage configuration if the number of port is different from 0
      */
      if (mstor->sclients_nb != 0)
      {
        ret = storcli_sup_send_lbg_port_configuration(STORCLI_LBG_ADD, (void *) mstor);
//        ret = storaged_lbg_initialize(mstor);
        if (ret < 0)
        {
          goto fatal;                       
        }
      }
      /*
      ** init of the cid/sid<-->lbg_id association table
      */
      for (i = 0; i < mstor->sids_nb;i++)
      {
        rozofs_storcli_cid_table_insert(mstor->cids[i],mstor->sids[i],mstor->lbg_id);       
      }
      /* Release mclient*/
      mclient_release(&mclt);
      /*
      ** starts the storaged connexion supervision threard
      */
      {
        pthread_t thread;

        if ((errno = pthread_create(&thread, NULL, connect_storage, mstor)) != 0) {
            severe("can't create connexion thread: %s", strerror(errno));
            goto fatal;
        }
        mstor->thread_started = 1;
      }
    }
    return 0;
    
fatal:
    return -1;
}


/*__________________________________________________________________________
 */
/**  Periodic thread whose aim is to detect a change in the configuration
    of the export and then to reload the lastest configuration of the exportd
    
 *
 * @param exportd_context_p: pointer to the exportd Master data structure
 */
#define CONF_CONNECTION_THREAD_TIMESPEC  2

void *storcli_exportd_config_supervision_thread(void *exportd_context_p) {

 exportclt_t * clt = (exportclt_t*) exportd_context_p;
 ep_gateway_t  arg_poll;
 epgw_conf_stor_arg_t arg_conf;
 epgw_status_ret_t  *ret_poll_p;
 epgw_conf_ret_t   *ret_conf_p;
 storcli_conf_ctx_t *storcli_conf_ctx_p = &storcli_conf_ctx;
 int status = -1;
 int retry = 0;
 
 struct timespec ts = {CONF_CONNECTION_THREAD_TIMESPEC, 0};
 pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
 /*
 ** clear the statistics array
 */
 memset(&storcli_conf_ctx,0,sizeof(storcli_conf_ctx_t));
 
 for(;;)
 {
    ret_poll_p = NULL;
    ret_conf_p = NULL;
    
    /*
    ** step 1: poll the state of the current configuration of the exportd
    ** For that purpose the storlci provides the current hash value of the
    configuration
    */

    arg_poll.eid = 0;  /* NS*/
    arg_poll.hash_config  = exportd_configuration_file_hash;
    arg_poll.nb_gateways  = 0; /* NS */
    arg_poll.gateway_rank = storcli_get_site_number(); 

    status = -1;
    retry = 0;   
    ret_poll_p = NULL; 
    STORCLI_CONF_STATS_INC(storcli_conf_ctx_p,poll_counter);
    while ((retry++ < clt->retries) &&
            (!(clt->rpcclt.client) ||
            !(ret_poll_p = ep_poll_conf_1(&arg_poll, clt->rpcclt.client)))) {

        /*
        ** release the sock if already configured to avoid losing fd descriptors
        */
        rpcclt_release(&clt->rpcclt);
        
        if (rpcclt_initialize
                (&clt->rpcclt, clt->host, EXPORT_PROGRAM, EXPORT_VERSION,
                ROZOFS_RPC_BUFFER_SIZE, ROZOFS_RPC_BUFFER_SIZE, 0, clt->timeout) != 0) {
            rpcclt_release(&clt->rpcclt);
            STORCLI_CONF_STATS_NOK(storcli_conf_ctx_p,poll_counter);
            errno = EPROTO;
        }
    }
    /*
    ** Check if poll has been successful
    */
    if (ret_poll_p == 0) {
        errno = EPROTO;
        goto out;
    } 
    if (ret_poll_p->status_gw.status == EP_FAILURE) {
        STORCLI_CONF_STATS_NOK(storcli_conf_ctx_p,poll_counter);
        goto out;
    } 
    if (ret_poll_p->status_gw.status == EP_SUCCESS) {
        /*
        ** the configuration is synced, nothing more to do
        */
        STORCLI_CONF_STATS_OK(storcli_conf_ctx_p,poll_counter);
        storcli_conf_ctx_p->conf_state = STORCLI_CONF_SYNCED;
        goto out;
    } 
    if (ret_poll_p->status_gw.status != EP_NOT_SYNCED) {
        /*
        ** unexpected return code !!
        */
        STORCLI_CONF_STATS_NOK(storcli_conf_ctx_p,poll_counter);

        goto out;
    }
    STORCLI_CONF_STATS_OK(storcli_conf_ctx_p,poll_counter);
    storcli_conf_ctx_p->conf_state = STORCLI_CONF_NOT_SYNCED;
    /*
    ** OK, it looks like that the configuration is out of sync
    ** so reload it
    */
    status = -1;
    retry = 0; 
    ret_conf_p = NULL;   
    STORCLI_CONF_STATS_INC(storcli_conf_ctx_p,conf_counter);
    arg_conf.hdr.gateway_rank = storcli_get_site_number();
    arg_conf.path = clt->root; 
    while ((retry++ < clt->retries) &&
            (!(clt->rpcclt.client) ||
            !(ret_conf_p = ep_conf_storage_1(&arg_conf, clt->rpcclt.client)))) {

        /*
        ** release the sock if already configured to avoid losing fd descriptors
        */
        rpcclt_release(&clt->rpcclt);
        
        if (rpcclt_initialize
                (&clt->rpcclt, clt->host, EXPORT_PROGRAM, EXPORT_VERSION,
                ROZOFS_RPC_BUFFER_SIZE, ROZOFS_RPC_BUFFER_SIZE, 0, clt->timeout) != 0) {
            rpcclt_release(&clt->rpcclt);
            STORCLI_CONF_STATS_NOK(storcli_conf_ctx_p,conf_counter);
            errno = EPROTO;
        }
    }
    /*
    ** Check if poll has been successful
    */
    if (ret_conf_p == 0) {
        errno = EPROTO;
        goto out;
    } 
    if (ret_conf_p->status_gw.status == EP_FAILURE) {
        errno = ret_conf_p->status_gw.ep_conf_ret_t_u.error;
        STORCLI_CONF_STATS_NOK(storcli_conf_ctx_p,conf_counter);
        goto out;
    } 
    if (ret_conf_p->status_gw.status != EP_SUCCESS) {
        /*
        ** unexpected return code !!
        */
        STORCLI_CONF_STATS_NOK(storcli_conf_ctx_p,conf_counter);
        goto out;
    }
    /*
    ** OK, we have received the new storaged configuration
    ** When reload the configuration of the storaged we can add new
    ** storage, change the cid/sid but the system does not accept the remove
    ** of one storaged.
    */ 
    status = exportclt_reload_check_mstorage(ret_conf_p,clt);
    if (status < 0)
    {
        STORCLI_CONF_STATS_NOK(storcli_conf_ctx_p,conf_counter);
        goto out;    
    }
    /*
    ** all is fine
    */
    STORCLI_CONF_STATS_OK(storcli_conf_ctx_p,conf_counter);
    storcli_conf_ctx_p->conf_state = STORCLI_CONF_SYNCED;
    exportd_configuration_file_hash = ret_conf_p->status_gw.ep_conf_ret_t_u.export.hash_conf;
    
out: 
    if (ret_poll_p != NULL) xdr_free((xdrproc_t) xdr_epgw_status_ret_t, (char *) ret_poll_p);
    if (ret_conf_p != NULL) xdr_free((xdrproc_t) xdr_epgw_conf_ret_t, (char *) ret_conf_p);
    nanosleep(&ts, NULL); 
 }
}
        



/*__________________________________________________________________________
*/

/**
 *  API to start the exportd configuration supervision thread
 *
    The goal of that thread is to poll the master exportd for checking
    any change in the configuration and to reload the configuration
    when there is a change
    
 @param clt: pointer to the context that contains the information relation to the exportd local config.
 
 @retval 0 on success
 @retval -1 on error
 */
int rozofs_storcli_start_exportd_config_supervision_thread(exportclt_t * clt) {

     pthread_t thread;
     int status = -1;
     
     /*
     ** The clt given in the interface is the one that has been set to read the configuration from 
     ** export during initialization. Do not reset it.
     */

     if ((errno = pthread_create(&thread, NULL, storcli_exportd_config_supervision_thread, clt)) != 0) {
         severe("can't create connexion thread: %s", strerror(errno));
         return status;
     }
     return 0;

}
