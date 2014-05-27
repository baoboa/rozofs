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

 
#ifndef GEO_REPLICA_NORTH_INTF_H
#define GEO_REPLICA_NORTH_INTF_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <fcntl.h> 
#include <sys/un.h>             
#include <errno.h>  

#include <rozofs/rozofs.h>
#include <rozofs/common/log.h>
#include <rozofs/core/ruc_buffer_api.h>
#include <rozofs/core/ruc_list.h>
#include <rozofs/core/af_unix_socket_generic_api.h>
#include <rozofs/core/af_unix_socket_generic.h>
#include <rozofs/core/rozofs_tx_api.h>
#include <rozofs/core/rozofs_socket_family.h>
#include <rozofs/core/af_inet_stream_api.h>

 /**
* Buffers information
*/
extern int geo_replicat_buf_count;   /**< number of buffer allocated for read/write on north interface */
extern int geo_replicat_buf_sz;      /**<read:write buffer size on north interface */

extern void *geo_replicat_receive_buffer_pool_p ;  /**< reference of the read/write buffer pool */
extern void *geo_replicat_xmit_buffer_pool_p ;  /**< reference of the read/write buffer pool */

#define GEO_REPLICA_BUF_RECV_CNT 8
#define GEO_REPLICA_BUF_RECV_SZ  (1024*32) 

#define GEO_REPLICA_SLAVE_PORT 53000

/*
**____________________________________________________
*/
/**
   

  Creation of the north interface buffers (AF_INET)
  
@param     : read_write_buf_count : number of read/write buffer
@param     : read_write_buf_sz : size of a read/write buffer

@retval   : RUC_OK : done
@retval          RUC_NOK : out of memory
*/

int geo_replicat_north_interface_buffer_init(int read_write_buf_count,int read_write_buf_sz);

/*
**____________________________________________________
*/
/**  

  Creation of the north interface listening socket (AF_INET) of the non-blocking exportd slave
  
  @param host : IP address in dot notation or hostname
  @param port:    listening port value

   @retval 0 succcess
   @retval  < 0 error
*/
int geo_replicat_north_interface_init(char *host,uint16_t port);


#endif
