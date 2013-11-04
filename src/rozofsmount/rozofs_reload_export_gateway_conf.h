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

 
#ifndef ROZOFS_RELOAD_EXPORT_GATEWAY_CONFIG_H
#define ROZOFS_RELOAD_EXPORT_GATEWAY_CONFIG_H

#include <rozofs/rpc/eproto.h>

typedef enum 
{
  ROZOFS_CONF_UNKNOWN = 0,
  ROZOFS_CONF_NOT_SYNCED ,
  ROZOFS_CONF_SYNCED,
  ROZOFS_CONF_MAX,
} rozofs_conf_state_e;

typedef enum
{
  ROZOFS_STATS_ATTEMPT = 0,
  ROZOFS_STATS_SUCCESS,
  ROZOFS_STATS_FAILURE,
  ROZOFS_STATS_MAX
} rozofs_conf_stats_e;

typedef struct _rozofs_conf_stats_t
{
   uint64_t poll_counter[ROZOFS_STATS_MAX];
   uint64_t conf_counter[ROZOFS_STATS_MAX];
} rozofs_conf_stats_t;

typedef struct rozofs_conf_ctx_t
{
   rozofs_conf_state_e  conf_state;    /**< configuration  state */  
   rozofs_conf_stats_t stats;  /**< statistics */
} rozofs_conf_ctx_t;



#define ROZOFS_CONF_STATS_INC(p,cpt)  \
{\
   p->stats.cpt[ROZOFS_STATS_ATTEMPT]++;\
}

#define ROZOFS_CONF_STATS_OK(p,cpt)  \
{\
   p->stats.cpt[ROZOFS_STATS_SUCCESS]++;\
}

#define ROZOFS_CONF_STATS_NOK(p,cpt)  \
{\
   p->stats.cpt[ROZOFS_STATS_FAILURE]++;\
}


extern uint32_t rozofs_configuration_file_hash;  /**< hash value of the configuration file */
extern rozofs_conf_ctx_t rozofs_conf_ctx;      /**< statistics associated with exportd configuration polling and reload */

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
int rozofs_start_exportd_config_supervision_thread(exportclt_t * clt);

#endif
