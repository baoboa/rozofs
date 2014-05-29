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

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h> 
#include <errno.h>  
#include <stdarg.h>    
#include <string.h>  
#include <strings.h>
#include <semaphore.h>
#include <pthread.h>
#include <config.h>

#include <rozofs/rozofs.h>
#include <rozofs/common/log.h>
#include <rozofs/rpc/export_profiler.h>
#include <rozofs/common/profile.h>
#include <rozofs/core/ruc_common.h>
#include <rozofs/core/ruc_sockCtl_api.h>
#include <rozofs/core/ruc_timer_api.h>
#include <rozofs/core/uma_tcp_main_api.h>
#include <rozofs/core/uma_dbg_api.h>
#include <rozofs/core/ruc_tcpServer_api.h>
#include <rozofs/core/ruc_tcp_client_api.h>
#include <rozofs/core/uma_well_known_ports_api.h>
#include <rozofs/core/af_unix_socket_generic_api.h>
#include <rozofs/core/north_lbg_api.h>
#include <rozofs/core/ruc_list.h>
#include <rozofs/core/rozofs_tx_common.h>
#include <rozofs/core/rozofs_tx_api.h>
#include <rozofs/core/af_unix_socket_generic_api.h>
#include <rozofs/core/rozofs_rpc_non_blocking_generic.h>
#include <rozofs/core/ruc_buffer_debug.h>
#include <rozofs/rpc/eproto.h>
#include <rozofs/rpc/epproto.h>
#include <rozofs/rozofs_debug_ports.h>
#include <rozofs/core/rozofs_rpc_non_blocking_generic_srv.h>
#include "export.h"
#include "export_expgateway_conf.h"
#include "export_north_intf.h"
#include "export_share.h"
#include "mdirent.h"
#include "geo_replication.h"
#include "geo_replica_srv.h"
#include "geo_replica_ctx.h"

DECLARE_PROFILING(epp_profiler_t);

int short_display = 0;


void * decoded_rpc_buffer_pool = NULL;

/*
**_________________________________________________________________________
*      PUBLIC FUNCTIONS
**_________________________________________________________________________
*/


#define SHOW_PROFILER_PROBE(probe) if (prof->probe[P_COUNT]) pChar += sprintf(pChar," %-24s | %15"PRIu64" | %9"PRIu64" | %18"PRIu64" | %15s |\n",\
                    #probe,\
                    prof->probe[P_COUNT],\
                    prof->probe[P_COUNT]?prof->probe[P_ELAPSE]/prof->probe[P_COUNT]:0,\
                    prof->probe[P_ELAPSE]," ");

#define SHOW_PROFILER_PROBE_BYTE(probe) if (prof->probe[P_COUNT]) pChar += sprintf(pChar," %-24s | %15"PRIu64" | %9"PRIu64" | %18"PRIu64" | %15"PRIu64" |\n",\
                    #probe,\
                    prof->probe[P_COUNT],\
                    prof->probe[P_COUNT]?prof->probe[P_ELAPSE]/prof->probe[P_COUNT]:0,\
                    prof->probe[P_ELAPSE],\
                    prof->probe[P_BYTES]);

char * show_profiler_one(char * pChar, uint32_t eid) {
    export_one_profiler_t * prof;   

    if (eid>EXPGW_EID_MAX_IDX) return pChar;
    if (eid != 0)
    {
      if (exportd_is_master()==0)
      {
	if (exportd_is_eid_match_with_instance(eid) ==0) return pChar;
      }  
    }  
    prof = export_profiler[eid];
    if (prof == NULL) return pChar;
        

    // Compute uptime for storaged process
    pChar +=  sprintf(pChar, "_______________________ EID = %d _______________________ \n",eid);
    pChar += sprintf(pChar, "   procedure              |     count       |  time(us) | cumulated time(us) |     bytes       |\n");
    pChar += sprintf(pChar, "--------------------------+-----------------+-----------+--------------------+-----------------+\n");
    SHOW_PROFILER_PROBE(ep_mount);
    SHOW_PROFILER_PROBE(ep_umount);
    SHOW_PROFILER_PROBE(ep_statfs);
    SHOW_PROFILER_PROBE(ep_lookup);
    SHOW_PROFILER_PROBE(ep_getattr);
    SHOW_PROFILER_PROBE(ep_setattr);
    SHOW_PROFILER_PROBE(ep_readlink);
    SHOW_PROFILER_PROBE(ep_mknod);
    SHOW_PROFILER_PROBE(ep_mkdir);
    SHOW_PROFILER_PROBE(ep_unlink);
    SHOW_PROFILER_PROBE(ep_rmdir);
    SHOW_PROFILER_PROBE(ep_symlink);
    SHOW_PROFILER_PROBE(ep_rename);
    SHOW_PROFILER_PROBE(ep_readdir);
    SHOW_PROFILER_PROBE_BYTE(ep_read_block);
    SHOW_PROFILER_PROBE_BYTE(ep_write_block);
    SHOW_PROFILER_PROBE(ep_link);
    SHOW_PROFILER_PROBE(ep_setxattr);
    SHOW_PROFILER_PROBE(ep_getxattr);
    SHOW_PROFILER_PROBE(ep_removexattr);
    SHOW_PROFILER_PROBE(ep_listxattr);
    if (short_display == 0) {
      SHOW_PROFILER_PROBE(export_lv1_resolve_entry);
      SHOW_PROFILER_PROBE(export_lv2_resolve_path);
      SHOW_PROFILER_PROBE(export_lookup_fid);
      SHOW_PROFILER_PROBE(export_update_files);
      SHOW_PROFILER_PROBE(export_update_blocks);
      SHOW_PROFILER_PROBE(export_stat);
      SHOW_PROFILER_PROBE(export_lookup);
      SHOW_PROFILER_PROBE(export_getattr);
      SHOW_PROFILER_PROBE(export_setattr);
      SHOW_PROFILER_PROBE(export_link);
      SHOW_PROFILER_PROBE(export_mknod);
      SHOW_PROFILER_PROBE(export_mkdir);
      SHOW_PROFILER_PROBE(export_unlink);
      SHOW_PROFILER_PROBE(export_rmdir);
      SHOW_PROFILER_PROBE(export_symlink);
      SHOW_PROFILER_PROBE(export_readlink);
      SHOW_PROFILER_PROBE(export_rename);
      SHOW_PROFILER_PROBE_BYTE(export_read);
      SHOW_PROFILER_PROBE(export_read_block);
      SHOW_PROFILER_PROBE(export_write_block);
      SHOW_PROFILER_PROBE(export_setxattr);
      SHOW_PROFILER_PROBE(export_getxattr);
      SHOW_PROFILER_PROBE(export_removexattr);
      SHOW_PROFILER_PROBE(export_listxattr);
      SHOW_PROFILER_PROBE(export_readdir);
      SHOW_PROFILER_PROBE(lv2_cache_put);
      SHOW_PROFILER_PROBE(lv2_cache_get);
      SHOW_PROFILER_PROBE(lv2_cache_del);
      SHOW_PROFILER_PROBE(volume_balance);
      SHOW_PROFILER_PROBE(volume_distribute);
      SHOW_PROFILER_PROBE(volume_stat);
      SHOW_PROFILER_PROBE(mdir_open);
      SHOW_PROFILER_PROBE(mdir_close);
      SHOW_PROFILER_PROBE(mdir_read_attributes);
      SHOW_PROFILER_PROBE(mdir_write_attributes);
      SHOW_PROFILER_PROBE(mreg_open);
      SHOW_PROFILER_PROBE(mreg_close);
      SHOW_PROFILER_PROBE(mreg_read_attributes);
      SHOW_PROFILER_PROBE(mreg_write_attributes);
      SHOW_PROFILER_PROBE(mreg_read_dist);
      SHOW_PROFILER_PROBE(mreg_write_dist);
      SHOW_PROFILER_PROBE(mslnk_open);
      SHOW_PROFILER_PROBE(mslnk_close);
      SHOW_PROFILER_PROBE(mslnk_read_attributes);
      SHOW_PROFILER_PROBE(mslnk_write_attributes);
      SHOW_PROFILER_PROBE(mslnk_read_link);
      SHOW_PROFILER_PROBE(mslnk_write_link);
    }
    SHOW_PROFILER_PROBE(get_mdirentry);
    SHOW_PROFILER_PROBE(put_mdirentry);
    SHOW_PROFILER_PROBE(del_mdirentry);
    SHOW_PROFILER_PROBE(list_mdirentries);
    SHOW_PROFILER_PROBE(gw_invalidate);
    SHOW_PROFILER_PROBE(gw_invalidate_all);
    SHOW_PROFILER_PROBE(gw_configuration);
    SHOW_PROFILER_PROBE(gw_poll);
    if (short_display == 0) {
      SHOW_PROFILER_PROBE(ep_configuration);
      SHOW_PROFILER_PROBE(ep_conf_gateway);
      SHOW_PROFILER_PROBE(ep_poll);
      SHOW_PROFILER_PROBE(export_clearclient_flock);
      SHOW_PROFILER_PROBE(export_clearowner_flock);
      SHOW_PROFILER_PROBE(export_set_file_lock);
      SHOW_PROFILER_PROBE(export_get_file_lock);
      SHOW_PROFILER_PROBE(export_poll_file_lock);
    }  
    SHOW_PROFILER_PROBE(ep_clearclient_flock);
    SHOW_PROFILER_PROBE(ep_clearowner_flock);
    SHOW_PROFILER_PROBE(ep_set_file_lock);
    SHOW_PROFILER_PROBE(ep_get_file_lock);
    SHOW_PROFILER_PROBE(ep_poll_file_lock);
    SHOW_PROFILER_PROBE(ep_geo_poll);
    return pChar;
}


static char * show_profiler_help(char * pChar) {
  pChar += sprintf(pChar,"usage:\n");
  pChar += sprintf(pChar,"profiler reset [ <eid> ] : reset statistics\n");
  pChar += sprintf(pChar,"profiler [ <eid> ]       : display statistics\n");  
  return pChar; 
}
void show_profiler(char * argv[], uint32_t tcpRef, void *bufRef) {
    char *pChar = uma_dbg_get_buffer();
    uint32_t eid;
    int ret;
    *pChar = 0;
    if (argv[1] == NULL) {
      for (eid=0; eid <= EXPGW_EID_MAX_IDX; eid++) 
        pChar = show_profiler_one(pChar,eid);
      uma_dbg_send(tcpRef, bufRef, TRUE, uma_dbg_get_buffer());   	  
      return;
    }

    if (strcmp(argv[1],"reset")==0) {

      if (argv[2] == NULL) {
	export_profiler_reset_all();
	uma_dbg_send(tcpRef, bufRef, TRUE, "Done\n");   
	return;	 
      }
      	     
      ret = sscanf(argv[2], "%d", &eid);
      if (ret != 1) {
        show_profiler_help(pChar);	
	uma_dbg_send(tcpRef, bufRef, TRUE, uma_dbg_get_buffer());   
	return;   
      }
      export_profiler_reset_one(eid);
      uma_dbg_send(tcpRef, bufRef, TRUE, "Done\n");   	  
      return;
    }

    ret = sscanf(argv[1], "%d", &eid);
    if (ret != 1) {
      show_profiler_help(pChar);	
      uma_dbg_send(tcpRef, bufRef, TRUE, uma_dbg_get_buffer());   
      return;   
    }
    pChar = show_profiler_one(pChar,eid);
    uma_dbg_send(tcpRef, bufRef, TRUE, uma_dbg_get_buffer());   	  
    return;
}



void show_profiler_conf(char * argv[], uint32_t tcpRef, void *bufRef) {
    uma_dbg_send(tcpRef, bufRef, TRUE, "unavailable");
}


void show_profiler_short(char * argv[], uint32_t tcpRef, void *bufRef) {
  short_display = 1;
  show_profiler(argv, tcpRef, bufRef);
  short_display = 0;  
}
/*
*_______________________________________________________________________
*/
/**
* dirent cache
*/
char *dirent_cache_display(char *pChar);

void show_dirent_cache(char * argv[], uint32_t tcpRef, void *bufRef) {
    char *pChar = uma_dbg_get_buffer();
    pChar = dirent_cache_display(pChar);
    pChar = dirent_disk_display_stats(pChar);
    pChar = dirent_wbcache_display_stats(pChar);
    uma_dbg_send(tcpRef, bufRef, TRUE, uma_dbg_get_buffer());   	  
}
/*
*_______________________________________________________________________
*/
/**
*   Storage,Volumes, EID statistics

  @param argv : standard argv[] params of debug callback
  @param tcpRef : reference of the TCP debug connection
  @param bufRef : reference of an output buffer 
  
  @retval none
*/
void show_vfstat(char * argv[], uint32_t tcpRef, void *bufRef) {
    char *pbuf = uma_dbg_get_buffer();
    int i, j;

    for (i = 0; i < gprofiler.nb_volumes; i++) {
        pbuf+=sprintf(pbuf, "Volume: %d georep: %s  Bsize: %d Blocks: %"PRIu64" Bfree: %"PRIu64" PercentFree: %d\n",
                gprofiler.vstats[i].vid,
                gprofiler.vstats[i].georep?"YES":"NO",
		 gprofiler.vstats[i].bsize,gprofiler.vstats[i].blocks, gprofiler.vstats[i].bfree,
                (int)((gprofiler.vstats[i].blocks==0)? 0:gprofiler.vstats[i].bfree*100/gprofiler.vstats[i].blocks));
        pbuf+=sprintf(pbuf, "\n%-6s | %-6s | %-20s | %-20s |\n", "Sid", "Status", "Capacity(B)","Free(B)");
        pbuf+=sprintf(pbuf, "-------+--------+----------------------+----------------------+\n");
        for (j = 0; j < gprofiler.vstats[i].nb_storages; j++) {
            pbuf+=sprintf(pbuf, "%6d | %-6s | %20"PRIu64" | %20"PRIu64" |\n", gprofiler.vstats[i].sstats[j].sid,
                    (gprofiler.vstats[i].sstats[j].status==1)?"UP":"DOWN", gprofiler.vstats[i].sstats[j].size,
                    gprofiler.vstats[i].sstats[j].free);
        }
	if (gprofiler.vstats[i].georep)
	{
          pbuf+=sprintf(pbuf, "-------+--------+----------------------+----------------------+\n");
	  int k = gprofiler.vstats[i].nb_storages;
          for (j = 0; j < gprofiler.vstats[i].nb_storages; j++) {
              pbuf+=sprintf(pbuf, "%6d | %-6s | %20"PRIu64" | %20"PRIu64" |\n", gprofiler.vstats[i].sstats[j+k].sid,
                      (gprofiler.vstats[i].sstats[j+k].status==1)?"UP":"DOWN", gprofiler.vstats[i].sstats[j+k].size,
                      gprofiler.vstats[i].sstats[j+k].free);
          }
	}

        pbuf += sprintf(pbuf, "\n%-6s | %-6s | %-20s | %-20s | %-12s | %-12s |\n", "Eid", "Bsize", "Blocks", "Bfree", "Files", "Ffree");
        pbuf += sprintf(pbuf, "-------+--------+----------------------+----------------------+--------------+--------------+\n");


        for (j = 0; j < gprofiler.nb_exports; j++) {

            if (gprofiler.estats[j].vid == gprofiler.vstats[i].vid)
                pbuf += sprintf(pbuf, "%6d | %6d | %20"PRIu64" | %20"PRIu64" | %12"PRIu64" | %12"PRIu64" |\n", gprofiler.estats[j].eid,
                    gprofiler.estats[j].bsize, gprofiler.estats[j].blocks, gprofiler.estats[j].bfree,
                    gprofiler.estats[j].files, gprofiler.estats[j].ffree);
        }
        pbuf += sprintf(pbuf, "\n");
    }

    uma_dbg_send(tcpRef, bufRef, TRUE, uma_dbg_get_buffer());
}
/*
*_______________________________________________________________________
*/
/**
*   Volumes statistics

  @param argv : standard argv[] params of debug callback
  @param tcpRef : reference of the TCP debug connection
  @param bufRef : reference of an output buffer 
  
  @retval none
*/
void show_vfstat_vol(char * argv[], uint32_t tcpRef, void *bufRef) {
    char *pbuf = uma_dbg_get_buffer();
    int i;

    for (i = 0; i < gprofiler.nb_volumes; i++) {
        pbuf+=sprintf(pbuf, "Volume: %d georep: %s  Bsize: %d Blocks: %"PRIu64" Bfree: %"PRIu64" PercentFree: %d\n",
                gprofiler.vstats[i].vid, 
		gprofiler.vstats[i].georep?"YES":"NO",
		gprofiler.vstats[i].bsize,gprofiler.vstats[i].blocks, gprofiler.vstats[i].bfree,
               (int)((gprofiler.vstats[i].blocks==0)? 0:gprofiler.vstats[i].bfree*100/gprofiler.vstats[i].blocks));

        pbuf+=sprintf(pbuf, "\n");
    }

    uma_dbg_send(tcpRef, bufRef, TRUE, uma_dbg_get_buffer());
}
/*
*_______________________________________________________________________
*/
/**
*   Storage statistics

  @param argv : standard argv[] params of debug callback
  @param tcpRef : reference of the TCP debug connection
  @param bufRef : reference of an output buffer 
  
  @retval none
*/
void show_vfstat_stor(char * argv[], uint32_t tcpRef, void *bufRef) {
    char *pbuf = uma_dbg_get_buffer();
    int i,j;

    for (i = 0; i < gprofiler.nb_volumes; i++) {
 
        pbuf+=sprintf(pbuf, "\n%-6s | %-6s | %-6s | %-6s | %-20s | %-20s | %-8s |\n","Vid", "Cid", "Sid", "Status", "Capacity(B)","Free(B)","Free(%)");
        pbuf+=sprintf(pbuf, "-------+--------+--------+--------+----------------------+----------------------+----------+\n");
        for (j = 0; j < gprofiler.vstats[i].nb_storages; j++) {
            pbuf+=sprintf(pbuf, "%6d | %6d | %6d | %-6s | %20"PRIu64" | %20"PRIu64" | %8d |\n",
                   gprofiler.vstats[i].vid,gprofiler.vstats[i].sstats[j].cid,
                   gprofiler.vstats[i].sstats[j].sid,
                   (gprofiler.vstats[i].sstats[j].status==1)?"UP":"DOWN", 
                   gprofiler.vstats[i].sstats[j].size,
                   gprofiler.vstats[i].sstats[j].free,
                   (int)((gprofiler.vstats[i].sstats[j].size==0)? 0:gprofiler.vstats[i].sstats[j].free*100/gprofiler.vstats[i].sstats[j].size));
        }
	if (gprofiler.vstats[i].georep)
	{
        pbuf+=sprintf(pbuf, "-------+--------+--------+--------+----------------------+----------------------+----------+\n");
	  int k = gprofiler.vstats[i].nb_storages;
          for (j = 0; j < gprofiler.vstats[i].nb_storages; j++) {
            pbuf+=sprintf(pbuf, "%6d | %6d | %6d | %-6s | %20"PRIu64" | %20"PRIu64" | %8d |\n",
                   gprofiler.vstats[i].vid,gprofiler.vstats[i].sstats[j+k].cid,
                   gprofiler.vstats[i].sstats[j+k].sid,
                   (gprofiler.vstats[i].sstats[j+k].status==1)?"UP":"DOWN", 
                   gprofiler.vstats[i].sstats[j+k].size,
                   gprofiler.vstats[i].sstats[j+k].free,
                   (int)((gprofiler.vstats[i].sstats[j+k].size==0)? 0:gprofiler.vstats[i].sstats[j+k].free*100/gprofiler.vstats[i].sstats[j+k].size));
          }
	}
        pbuf+=sprintf(pbuf, "\n");
    }

    uma_dbg_send(tcpRef, bufRef, TRUE, uma_dbg_get_buffer());
}

/*
*_______________________________________________________________________
*/
/**
*   Storage statistics for geo-replication

  @param argv : standard argv[] params of debug callback
  @param tcpRef : reference of the TCP debug connection
  @param bufRef : reference of an output buffer 
  
  @retval none
*/
void show_geo_vfstat_stor(char * argv[], uint32_t tcpRef, void *bufRef) {
    char *pbuf = uma_dbg_get_buffer();
    int i,j;
    
    sprintf(pbuf, "Empty\n");
    for (i = 0; i < gprofiler.nb_volumes; i++) {
       if (gprofiler.vstats[i].georep == 0) continue;
       int k = gprofiler.vstats[i].nb_storages;
 
        pbuf+=sprintf(pbuf, "\n%-6s | %-6s | %-6s | %-6s | %-6s |\n","Vid", "Cid", "Sid", "Local", "Remote");
        pbuf+=sprintf(pbuf, "-------+--------+--------+--------+--------+\n");
        for (j = 0; j < gprofiler.vstats[i].nb_storages; j++) {
            pbuf+=sprintf(pbuf, "%6d | %6d | %6d | %-6s | %-6s |\n",
                   gprofiler.vstats[i].vid,gprofiler.vstats[i].sstats[j].cid,
                   gprofiler.vstats[i].sstats[j].sid,
                   (gprofiler.vstats[i].sstats[j].status==1)?"UP":"DOWN", 
                   (gprofiler.vstats[i].sstats[j+k].status==1)?"UP":"DOWN");
        }
        pbuf+=sprintf(pbuf, "\n");
    }

    uma_dbg_send(tcpRef, bufRef, TRUE, uma_dbg_get_buffer());
}
/*
 *_______________________________________________________________________
 */
/**
*   EID statistics

  @param argv : standard argv[] params of debug callback
  @param tcpRef : reference of the TCP debug connection
  @param bufRef : reference of an output buffer 
  
  @retval none
*/
void show_vfstat_eid(char * argv[], uint32_t tcpRef, void *bufRef) {
    char *pbuf = uma_dbg_get_buffer();
    int j;

        pbuf+=sprintf(pbuf, "\n%-6s | %-6s | %-6s | %-20s | %-20s | %-12s | %-12s |\n", "Eid","Vid", "Bsize","Blocks", "Bfree", "Files", "Ffree");
        pbuf+=sprintf(pbuf, "-------+--------+--------+----------------------+----------------------+--------------+--------------+\n");


        for (j = 0; j < gprofiler.nb_exports; j++) {

                pbuf+=sprintf(pbuf, "%6d | %6d | %6d | %20"PRIu64" | %20"PRIu64" | %12"PRIu64" | %12"PRIu64" |\n", gprofiler.estats[j].eid,
                    gprofiler.estats[j].vid,
                    gprofiler.estats[j].bsize, gprofiler.estats[j].blocks, gprofiler.estats[j].bfree,
                    gprofiler.estats[j].files, gprofiler.estats[j].ffree);
        }
        pbuf+=sprintf(pbuf, "\n");

    uma_dbg_send(tcpRef, bufRef, TRUE, uma_dbg_get_buffer());
}
/*
 *_______________________________________________________________________
 */
/**
*   LV2 cache statistics

  @param argv : standard argv[] params of debug callback
  @param tcpRef : reference of the TCP debug connection
  @param bufRef : reference of an output buffer 
  
  @retval none
*/
void show_lv2_attribute_cache(char * argv[], uint32_t tcpRef, void *bufRef) {
  lv2_cache_display( &cache, uma_dbg_get_buffer());
  uma_dbg_send(tcpRef, bufRef, TRUE, uma_dbg_get_buffer());
}
/*
 *_______________________________________________________________________
 */
/**
*   file lock

  @param argv : standard argv[] params of debug callback
  @param tcpRef : reference of the TCP debug connection
  @param bufRef : reference of an output buffer 
  
  @retval none
*/
void show_flock(char * argv[], uint32_t tcpRef, void *bufRef) {
  display_file_lock(uma_dbg_get_buffer());
  uma_dbg_send(tcpRef, bufRef, TRUE, uma_dbg_get_buffer());
}

// For trace purpose
struct timeval     Global_timeDay;
unsigned long long Global_timeBefore, Global_timeAfter;

int volatile expgwc_non_blocking_thread_started;

/*
**
*/


uint32_t ruc_init(uint32_t test,uint16_t dbg_port,uint16_t exportd_instance) {
  int ret;


  uint32_t        mx_tcp_client = 2;
  uint32_t        mx_tcp_server = 2;
  uint32_t        mx_tcp_server_cnx = 10;
  uint32_t        mx_af_unix_ctx = 512;
  uint32_t        mx_lbg_north_ctx = 64;

//#warning TCP configuration ressources is hardcoded!!
  /*
  ** init of the system ticker
  */
  rozofs_init_ticker();
  /*
  ** trace buffer initialization
  */
  ruc_traceBufInit();
#if 1
 /*
 ** Not needed since there is already done
 ** by libUtil
 */

 /* catch the sigpipe signal for socket 
 ** connections with RELC(s) in this way when a RELC
 ** connection breaks an errno is set on a recv or send 
 **  socket primitive 
 */ 
  struct sigaction sigAction;
  
  sigAction.sa_flags=SA_RESTART;
  sigAction.sa_handler = SIG_IGN; /* Mask SIGPIPE */
  if(sigaction (SIGPIPE, &sigAction, NULL) < 0) 
  {
    exit(0);    
  }
#if 0
  sigAction.sa_flags=SA_RESTART;
  sigAction.sa_handler = hand; /*  */
  if(sigaction (SIGUSR1, &sigAction, NULL) < 0) 
  {
    exit(0);    
  }
#endif
#endif

   /*
   ** initialize the socket controller:
   **   for: NPS, Timer, Debug, etc...
   */
//#warning set the number of contexts for socketCtrl to 1024
   ret = ruc_sockctl_init(1024);
   if (ret != RUC_OK)
   {
     fatal( " socket controller init failed" );
   }

   /*
   **  Timer management init
   */
   ruc_timer_moduleInit(FALSE);

   while(1)
   {
     /*
     **--------------------------------------
     **  configure the number of TCP connection
     **  supported
     **--------------------------------------   
     **  
     */ 
     ret = uma_tcp_init(mx_tcp_client+mx_tcp_server+mx_tcp_server_cnx);
     if (ret != RUC_OK) break;

     /*
     **--------------------------------------
     **  configure the number of TCP server
     **  context supported
     **--------------------------------------   
     **  
     */    
     ret = ruc_tcp_server_init(mx_tcp_server);
     if (ret != RUC_OK) break;
#if 0
     /*
     **--------------------------------------
     **  configure the number of TCP client
     **  context supported
     **--------------------------------------   
     **  
     */    
     ret = ruc_tcp_clientinit(mx_tcp_client);
     if (ret != RUC_OK) break;   
#endif

     /*
     **--------------------------------------
     **  configure the number of AF_UNIX
     **  context supported
     **--------------------------------------   
     **  
     */    
     ret = af_unix_module_init(mx_af_unix_ctx,
                               2,1024*1, // xmit(count,size)
                               2,1024*1 // recv(count,size)
                               );
     if (ret != RUC_OK) break;   

     /*
     **--------------------------------------
     **  configure the number of Load Balancer
     **  contexts supported
     **--------------------------------------   
     **  
     */    
     ret = north_lbg_module_init(mx_lbg_north_ctx);
     if (ret != RUC_OK) break;   
     /*
     ** init of the rpc client module
     */
     
     ret = rozofs_rpc_module_init();
     if (ret != RUC_OK) break;   

     /*
     ** RPC SERVER MODULE INIT
     */
     ret = rozorpc_srv_module_init();
     if (ret != RUC_OK) break; 
     /*
     ** Init of the module that handles the configuration channel with main process of exportd
     */
     ret = expgwc_int_chan_moduleInit();
     if (ret != RUC_OK) break; 
     
     ret = rozofs_tx_module_init(EXPORTNB_SOUTH_TX_CNT,  // transactions count
                                 EXPORTNB_SOUTH_TX_RECV_BUF_CNT,EXPORTNB_SOUTH_TX_RECV_BUF_SZ,        // xmit small [count,size]
                                 EXPORTNB_SOUTH_TX_RECV_BUF_CNT,EXPORTNB_SOUTH_TX_RECV_BUF_SZ,  // xmit large [count,size]
                                 EXPORTNB_SOUTH_TX_RECV_BUF_CNT,EXPORTNB_SOUTH_TX_RECV_BUF_SZ,        // recv small [count,size]
                                 EXPORTNB_SOUTH_TX_RECV_BUF_CNT,EXPORTNB_SOUTH_TX_RECV_BUF_SZ);  // recv large [count,size];  
     break;
     

   }
   /*
   ** internal debug init
   */
   //ruc_debug_init();


     /*
     **--------------------------------------
     **   D E B U G   M O D U L E
     **--------------------------------------
     */

     uma_dbg_init(10,INADDR_ANY,dbg_port);

    {
        char name[32];
	if (exportd_is_master())
	{
          sprintf(name, "exportd-M");
	}
	else
	{
          sprintf(name, "exportd-S%d ",  exportd_instance);	
	}
        uma_dbg_set_name(name);
    }

//#warning Start of specific application initialization code
 

 return ret;
}



/**
*  Init of the data structure used for the non blocking entity

  @retval 0 on success
  @retval -1 on error
*/
int expgwc_non_blocking_init(uint16_t dbg_port, uint16_t exportd_instance) {
  int   ret;
//  sem_t semForEver;    /* semaphore for blocking the main thread doing nothing */


 ret = ruc_init(FALSE,dbg_port + RZDBG_EXPORTD_PORT,exportd_instance);
 
 if (ret != RUC_OK) return -1;
 
 export_expgw_conf_moduleInit();
 
 return 0;

}


/*
 *_______________________________________________________________________
 */

/**
 *  This function is the entry point for setting rozofs in non-blocking mode

   @param args->ch: reference of the fuse channnel
   @param args->se: reference of the fuse session
   @param args->max_transactions: max number of transactions that can be handled in parallel
   
   @retval -1 on error
   @retval : no retval -> only on fatal error

 */
int expgwc_start_nb_blocking_th(void *args) {


    int ret;
    int size;
    //sem_t semForEver;    /* semaphore for blocking the main thread doing nothing */
    exportd_start_conf_param_t *args_p = (exportd_start_conf_param_t*)args;
 
    /*
    ** set the uptime
    */
    gprofiler.uptime = time(0);
    
    ret = expgwc_non_blocking_init(args_p->debug_port, args_p->instance);
    if (ret != RUC_OK) {
        /*
         ** fatal error
         */
         fatal("can't initialize non blocking thread");
        return -1;
    }
    /*
    ** create the shared memory for exportd slaves
    */

    export_sharemem_create_or_attach(args_p);
    /*
    ** Create a buffer pool to decode spproto RPC requests
    */
    union {
	    ep_path_t ep_mount_1_arg;
	    uint32_t ep_umount_1_arg;
	    uint32_t ep_statfs_1_arg;
	    epgw_lookup_arg_t ep_lookup_1_arg;
	    epgw_mfile_arg_t ep_getattr_1_arg;
	    epgw_setattr_arg_t ep_setattr_1_arg;
	    epgw_mfile_arg_t ep_readlink_1_arg;
	    epgw_mknod_arg_t ep_mknod_1_arg;
	    epgw_mkdir_arg_t ep_mkdir_1_arg;
	    epgw_unlink_arg_t ep_unlink_1_arg;
	    epgw_rmdir_arg_t ep_rmdir_1_arg;
	    epgw_symlink_arg_t ep_symlink_1_arg;
	    epgw_rename_arg_t ep_rename_1_arg;
	    epgw_readdir_arg_t ep_readdir_1_arg;
	    epgw_io_arg_t ep_read_block_1_arg;
	    epgw_write_block_arg_t ep_write_block_1_arg;
	    epgw_link_arg_t ep_link_1_arg;
	    epgw_setxattr_arg_t ep_setxattr_1_arg;
	    epgw_getxattr_arg_t ep_getxattr_1_arg;
	    epgw_removexattr_arg_t ep_removexattr_1_arg;
	    epgw_listxattr_arg_t ep_listxattr_1_arg;
	    uint16_t ep_list_cluster_1_arg;
	    ep_path_t ep_conf_storage_1_arg;
	    ep_gateway_t ep_poll_conf_1_arg;
	    ep_path_t ep_conf_expgw_1_arg;
	    epgw_lock_arg_t ep_set_file_lock_1_arg;
	    epgw_lock_arg_t ep_get_file_lock_1_arg;
	    epgw_lock_arg_t ep_clear_owner_file_lock_1_arg;
	    epgw_lock_arg_t ep_clear_client_file_lock_1_arg;
	    epgw_lock_arg_t ep_poll_file_lock_1_arg;
    } argument;
    size = sizeof(argument);
    decoded_rpc_buffer_pool = ruc_buf_poolCreate(ROZORPC_SRV_CTX_CNT,size);
    if (decoded_rpc_buffer_pool == NULL) {
      fatal("Can not allocate decoded_rpc_buffer_pool");
      return -1;
    }
    ruc_buffer_debug_register_pool("rpcDecodedRequest",decoded_rpc_buffer_pool);    

    /*
    ** Init of the north interface (read/write request processing)
    */ 
    ret = expnb_north_interface_buffer_init(EXPNB_BUF_RECV_CNT, EXPNB_BUF_RECV_SZ);
    if (ret < 0) {
      fatal("Fatal error on storio_north_interface_buffer_init()\n");
      return -1;
    }
    ret = expnb_north_interface_init(args_p->exportd_hostname,EXPNB_SLAVE_PORT+args_p->instance);
    if (ret < 0) {
      fatal("Fatal error on expnb_north_interface_init()\n");
      return -1;
    }
    /*
    ** add profiler subject (exportd statistics)
    */
    uma_dbg_addTopic("profiler", show_profiler);
    uma_dbg_addTopic("profiler_conf", show_profiler_conf);
    uma_dbg_addTopic("profiler_short", show_profiler_short);
    /*
    ** dirent cache stats
    */
    uma_dbg_addTopic("dirent_cache",show_dirent_cache);
    uma_dbg_addTopic("dirent_wbthread",show_wbcache_thread);
    /*
    ** do not provide volume stats for the case of the slaves
    */
    if (args_p->slave == 0)
    {
      uma_dbg_addTopic("vfstat", show_vfstat);
      uma_dbg_addTopic("vfstat_stor",show_vfstat_stor);
      uma_dbg_addTopic("vfstat_vol",show_vfstat_vol);
      uma_dbg_addTopic("vfstat_exp",show_vfstat_eid);
      uma_dbg_addTopic("exp_slave", show_export_slave);
    }
    uma_dbg_addTopic("lv2_cache",show_lv2_attribute_cache);
    uma_dbg_addTopic("flock",    show_flock);    
    uma_dbg_addTopic("trk_thread", show_tracking_thread);
    
    if (args_p->slave == 0)
    {
      /*
      ** init of the server part of the geo-replication service
      */
      ret = geo_replicat_rpc_srv_init(args);
      if (ret < 0)
      {
	severe("geo replication service is unavailable: %s",strerror(errno));
      }
      uma_dbg_addTopic("geo_profiler", show_geo_profiler);
    }
    
    ret = geo_proc_module_init(GEO_REP_SRV_CLI_CTX_MAX);
    if (ret < 0)
    {
      severe("geo replication service is unavailable: %s",strerror(errno));
    }
    uma_dbg_addTopic("geo_replica", show_geo_replication);

    expgwc_non_blocking_thread_started = 1;
    
    info("exportd non-blocking thread started (instance: %d, port: %d).",
            args_p->instance, args_p->debug_port);
    /*
     ** main loop
     */
    export_sharemem_change_state("running");
    while (1) {
        ruc_sockCtrl_selectWait();
    }
}


