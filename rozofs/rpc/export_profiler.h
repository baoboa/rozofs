/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _EXPORT_PROFILER_H
#define _EXPORT_PROFILER_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>

#include <rozofs/rozofs.h>


struct export_one_profiler_t {
  uint64_t ep_mount[2];
  uint64_t ep_umount[2];
  uint64_t ep_statfs[2];
  uint64_t ep_lookup[2];
  uint64_t ep_getattr[2];
  uint64_t ep_setattr[2];
  uint64_t ep_readlink[2];
  uint64_t ep_mknod[2];
  uint64_t ep_mkdir[2];
  uint64_t ep_unlink[2];
  uint64_t ep_rmdir[2];
  uint64_t ep_symlink[2];
  uint64_t ep_rename[2];
  uint64_t ep_readdir[2];
  uint64_t ep_read_block[3];
  uint64_t ep_write_block[3];
  uint64_t ep_link[2];
  uint64_t ep_setxattr[2];
  uint64_t ep_getxattr[2];
  uint64_t ep_removexattr[2];
  uint64_t ep_listxattr[2];
  uint64_t export_lv1_resolve_entry[2];
  uint64_t export_lv2_resolve_path[2];
  uint64_t export_lookup_fid[2];
  uint64_t export_update_files[2];
  uint64_t export_update_blocks[2];
  uint64_t export_stat[2];
  uint64_t export_lookup[2];
  uint64_t export_getattr[2];
  uint64_t export_setattr[2];
  uint64_t export_link[2];
  uint64_t export_mknod[2];
  uint64_t export_mkdir[2];
  uint64_t export_unlink[2];
  uint64_t export_rmdir[2];
  uint64_t export_symlink[2];
  uint64_t export_readlink[2];
  uint64_t export_rename[2];
  uint64_t export_read[3];
  uint64_t export_read_block[2];
  uint64_t export_write_block[2];
  uint64_t export_setxattr[2];
  uint64_t export_getxattr[2];
  uint64_t export_removexattr[2];
  uint64_t export_listxattr[2];
  uint64_t export_readdir[2];
  uint64_t lv2_cache_put[2];
  uint64_t lv2_cache_get[2];
  uint64_t lv2_cache_del[2];
  uint64_t volume_balance[2];
  uint64_t volume_distribute[2];
  uint64_t volume_stat[2];
  uint64_t mdir_open[2];
  uint64_t mdir_close[2];
  uint64_t mdir_read_attributes[2];
  uint64_t mdir_write_attributes[2];
  uint64_t mreg_open[2];
  uint64_t mreg_close[2];
  uint64_t mreg_read_attributes[2];
  uint64_t mreg_write_attributes[2];
  uint64_t mreg_read_dist[2];
  uint64_t mreg_write_dist[2];
  uint64_t mslnk_open[2];
  uint64_t mslnk_close[2];
  uint64_t mslnk_read_attributes[2];
  uint64_t mslnk_write_attributes[2];
  uint64_t mslnk_read_link[2];
  uint64_t mslnk_write_link[2];
  uint64_t get_mdirentry[2];
  uint64_t put_mdirentry[2];
  uint64_t del_mdirentry[2];
  uint64_t list_mdirentries[2];
  uint64_t gw_invalidate[2];
  uint64_t gw_invalidate_all[2];
  uint64_t gw_configuration[2];
  uint64_t gw_poll[2];
  uint64_t ep_configuration[2];
  uint64_t ep_conf_gateway[2];
  uint64_t ep_poll[2];
  uint64_t export_clearclient_flock[2];
  uint64_t export_clearowner_flock[2];
  uint64_t export_set_file_lock[2];
  uint64_t export_get_file_lock[2];
  uint64_t export_poll_file_lock[2];
  uint64_t ep_clearclient_flock[2];
  uint64_t ep_clearowner_flock[2];
  uint64_t ep_set_file_lock[2];
  uint64_t ep_get_file_lock[2];
  uint64_t ep_poll_file_lock[2];
  uint64_t ep_geo_poll[2];
};
typedef struct export_one_profiler_t export_one_profiler_t;


extern export_one_profiler_t * export_profiler[];
extern uint32_t                export_profiler_eid;

#define START_PROFILING(the_probe)\
    uint64_t tic=0, toc;\
    struct timeval tv;\
    if (export_profiler_eid <= EXPGW_EXPORTD_MAX_IDX) {\
       export_one_profiler_t * prof = export_profiler[export_profiler_eid];\
       if (prof != NULL) {\
          prof->the_probe[P_COUNT]++;\
          gettimeofday(&tv,(struct timezone *)0);\
          tic = MICROLONG(tv);\
       }\
    }

#define STOP_PROFILING(the_probe)\
    if (export_profiler_eid <= EXPGW_EXPORTD_MAX_IDX) {\
       export_one_profiler_t * prof = export_profiler[export_profiler_eid];\
       if (prof != NULL) {\
          gettimeofday(&tv,(struct timezone *)0);\
          toc = MICROLONG(tv);\
          prof->the_probe[P_ELAPSE] += (toc - tic);\
       }\
    }

#define START_PROFILING_IO(the_probe, the_bytes)\
    uint64_t tic=0, toc;\
    struct timeval tv;\
    if (export_profiler_eid <= EXPGW_EXPORTD_MAX_IDX) {\
       export_one_profiler_t * prof = export_profiler[export_profiler_eid];\
       if (prof != NULL) {\
          prof->the_probe[P_COUNT]++;\
          gettimeofday(&tv,(struct timezone *)0);\
          tic = MICROLONG(tv);\
          prof->the_probe[P_BYTES] += the_bytes;\
       }\
    }  
#define GET_EID_PROB_COUNT(val,eid,the_probe)\
    val = 0;\
    if (eid <= EXPGW_EXPORTD_MAX_IDX) {\
       export_one_profiler_t * prof = export_profiler[eid];\
       if (prof != NULL) {\
          val = prof->the_probe[P_COUNT];\
       }\
    }        
/*
*________________________________________________
* Allocate memory for export profiler statistics
*
* @param eid The export identifier
*
* @etval 0 on success, -1 on failure
*/    
static inline int export_profiler_allocate(int eid) {

  if (eid>EXPGW_EXPORTD_MAX_IDX) return -1;
  
  if (export_profiler[eid] != NULL) {
    free(export_profiler[eid]);
    export_profiler[eid] = NULL;
  }
  
  export_profiler[eid] = malloc(sizeof(export_one_profiler_t));
  if (export_profiler[eid] == NULL) {
    return -1;	    
  }
  memset(export_profiler[eid],0,sizeof(export_one_profiler_t));
  return 0;
}  
/*
*________________________________________________
* Reset statistics of an export id
*
* @param eid The export identifier
*/    
static inline void export_profiler_reset_one(int eid) {

  if (eid>EXPGW_EXPORTD_MAX_IDX) return;
  
  if (export_profiler[eid] != NULL) {
    memset(export_profiler[eid],0,sizeof(export_one_profiler_t));
  }
} 
/*  
*________________________________________________
* Reset statistics of all export ids

*/    
static inline void export_profiler_reset_all() {
  int eid;
  for (eid=0; eid <= EXPGW_EXPORTD_MAX_IDX; eid++) {
    if (export_profiler[eid] != NULL) {
      memset(export_profiler[eid],0,sizeof(export_one_profiler_t));
    }    
  }
}   
/*
*________________________________________________
* Free allocated memory for export profiler statistics
*
* @param eid The export identifier
*/    
static inline void export_profiler_free(int eid) {

  if (eid>EXPGW_EXPORTD_MAX_IDX) return;
  
  if (export_profiler[eid] != NULL) {
    free(export_profiler[eid]);
    export_profiler[eid] = NULL;
  }
} 
#ifdef __cplusplus
}
#endif

#endif 
