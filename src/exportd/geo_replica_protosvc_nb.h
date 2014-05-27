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

#ifndef GEO_REPLICA_PROTOSVC_NB_H
#define GEO_REPLICA_PROTOSVC_NB_H

#include <stdint.h>

#include <config.h>
#include <rozofs/rozofs.h>
#include <rozofs/core/rozofs_rpc_non_blocking_generic_srv.h>
#include <rozofs/core/ruc_buffer_api.h>
#include <rozofs/rpc/rozofs_rpc_util.h>
#include <rozofs/common/profile.h>
#include <rozofs/rpc/geo_replica_proto.h>
#include "geo_replica_proto_nb.h"
extern void * decoded_rpc_buffer_pool;

/*
**__________________________________________________________________________
*/
/**
  Server callback  for geo replication protocol:

  That callback is called upon receiving a GW_PROGRAM message
  from the master exportd

    
  @param socket_ctx_p: pointer to the af unix socket
  @param socketId: reference of the socket (not used)
 
   @retval : TRUE-> xmit ready event expected
  @retval : FALSE-> xmit  ready event not expected
*/
void geo_replica_req_rcv_cbk(void *userRef,uint32_t  socket_ctx_idx, void *recv_buf);

#endif
