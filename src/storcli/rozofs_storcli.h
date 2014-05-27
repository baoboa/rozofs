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

#ifndef ROZOFS_STORCLI_H
#define ROZOFS_STORCLI_H


#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <errno.h>



#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <rozofs/core/ruc_list.h>
#include <errno.h>


#include <rozofs/rozofs.h>
#include "config.h"
#include <rozofs/common/log.h>
#include <rozofs/common/xmalloc.h>
#include <rozofs/common/dist.h>
#include "storage_proto.h"
#include <rozofs/core/rozofs_tx_common.h>
#include "rozofs_storcli_transform.h"
#include <rozofs/core/north_lbg_api.h>
#include <rozofs/rpc/sproto.h>
#include <rozofs/rpc/storcli_proto.h>
#include <rpc/rpc.h>
#include <rozofs/common/profile.h>
#include <rozofs/rpc/stcpproto.h>
#include "storcli_ring.h"


 
#ifndef TEST_STORCLI_TEST
#define NORTH_LBG_GET_STATE north_lbg_get_state
#else
int test_north_lbg_get_state(int lbg_idx);
#define NORTH_LBG_GET_STATE test_north_lbg_get_state
#endif

extern uint64_t storcli_buf_depletion_count; /**< buffer depletion on storcli buffers */
extern uint64_t storcli_rng_full_count; /**< ring request full counter */

/**
*  STORCLI Resource configuration
*/
#define ROZOFS_MAX_LAYOUT 16
#define STORCLI_CNF_NO_BUF_CNT          1
#define STORCLI_CNF_NO_BUF_SZ           1

/**
*  North Interface
*/
#define STORCLI_CTX_CNT 64 /**< context for processing either a read or write request from rozofsmount and internal read req */
#define STORCLI_CTX_MIN_CNT 16 /**< minimum count to process a request from rozofsmount */
/**
* Buffer s associated with the reception of the load balancing group on north interface
*/
#define STORCLI_NORTH_LBG_BUF_RECV_CNT   STORCLI_CTX_CNT  /**< number of reception buffer to receive from rozofsmount */
#define STORCLI_NORTH_LBG_BUF_RECV_SZ    (1024*258)  /**< max user data payload is 256K       */
/**
* Storcli buffer mangement configuration
*  INTERNAL_READ are small buffer used when a write request requires too trigger read of first and/or last block
*/
#define STORCLI_NORTH_MOD_INTERNAL_READ_BUF_CNT   STORCLI_CTX_CNT  /**< rozofs_storcli_north_small_buf_count  */
#define STORCLI_NORTH_MOD_INTERNAL_READ_BUF_SZ   1024  /**< rozofs_storcli_north_small_buf_sz  */

#define STORCLI_NORTH_MOD_XMIT_BUF_CNT   STORCLI_CTX_CNT  /**< rozofs_storcli_north_large_buf_count  */
#define STORCLI_NORTH_MOD_XMIT_BUF_SZ    STORCLI_NORTH_LBG_BUF_RECV_SZ  /**< rozofs_storcli_north_large_buf_sz  */

#define STORCLI_SOUTH_TX_XMIT_BUF_CNT   (STORCLI_CTX_CNT*ROZOFS_MAX_LAYOUT)  /**< rozofs_storcli_south_large_buf_count  */
#define STORCLI_SOUTH_TX_XMIT_BUF_SZ    (1024*160)                           /**< rozofs_storcli_south_large_buf_sz  */

/**
* configuartion of the resource of the transaction module
*
*  concerning the buffers, only reception buffer are required 
*/

#define STORCLI_SOUTH_TX_CNT   (STORCLI_CTX_CNT*ROZOFS_MAX_LAYOUT)  /**< number of transactions towards storaged  */
#define STORCLI_SOUTH_TX_RECV_BUF_CNT   STORCLI_SOUTH_TX_CNT  /**< number of recption buffers  */
#define STORCLI_SOUTH_TX_RECV_BUF_SZ    STORCLI_SOUTH_TX_XMIT_BUF_SZ  /**< buffer size  */

#define STORCLI_NORTH_TX_BUF_CNT   0  /**< not needed for the case of storcli  */
#define STORCLI_NORTH_TX_BUF_SZ    0  /**< buffer size  */
/**
*  level 1 hash table size: that hash table is used to queue the storcli request in order to implement
*  per fid serialization 
*/
#define STORCLI_HASH_SIZE STORCLI_CTX_CNT    /**<  level 1 storcli hash table size for per fid serialisattion */
#define STORCLI_DO_NOT_QUEUE  1
#define STORCLI_DO_QUEUE  0

/**
* write structure
*
*  note that the ROZOFS_WR_FIRST and ROZOFS_WR_LAST might nit exist.They exist for the following case:
   FIRST: the offset to read does not start on a ROZOFS_BSIZE boundary
   LAST : the offset+len does ends of a ROZOFS_BSIZE and offset+len< file_size
   For these 2 cases, rozofs MUST initiate a read a ROZOFS_BSIZE size and perform the buffer adjustement
   by copying the missing bytes in the original write buffer.
*/
typedef enum
{
  ROZOFS_WR_FIRST = 0,  /**< first block   */
  ROZOFS_WR_MIDDLE,     /**< middle block  */
  ROZOFS_WR_LAST,       /**< last block    */
  ROZOFS_WR_MAX
} rozofs_write_buf_type_e;



typedef enum
{
  ROZOFS_WR_ST_IDLE = 0,          /**< no action: idle state entry is not used  */
  ROZOFS_WR_ST_TRANSFORM_REQ,     /**< transform request  */
  ROZOFS_WR_ST_TRANSFORM_DONE,    /**< transform done  */
  ROZOFS_WR_ST_RD_REQ,            /**< request for reading a rozofs block  */
  ROZOFS_WR_ST_ERROR,             /**< state reached upon a failure on read  */
  ROZOFS_WR_ST_MAX
} rozofs_write_state_e;



#define ROZOFS_STORCLI_MAX_RETRY   3  /**< max attempts for read or write a projection on a storage */
/**
* structure used to handle projection construction
*/
typedef struct _rozofs_storcli_ingress_write_buf_t
{
   uint32_t transaction_id;       /**< current transaction_id (non null) for FISRT and LAST only          */   
   uint32_t len;       /**< always a multiple of  ROZOFS_BSIZE or 0                                       */   
   uint32_t state;     /**< see rozofs_write_state_e                                                      */   
   int      errcode;   /**< errcode in case of failure      */
   uint64_t off;       /**< offset in the file                                                            */
   void    *read_buf;  /**< ruc buffer that contains the payload or NULL (valid for fisrt and last only)  */ 
   char    *data;      /**< pointer to data payload                                                       */
   /* following parameter are used by transform  */
   uint32_t first_block_idx;  /**< relative index of the first block to transform    */   
   uint32_t number_of_blocks; /**< number blocks to transform                        */   
   uint32_t last_block_size;  /**< effective size of the last block: written in the header of the last projection     */
} rozofs_storcli_ingress_write_buf_t;

/**
* structure used to keep track of the associated between a storage and a projection
*/
typedef struct _rozofs_storcli_lbg_prj_assoc_t
{
  uint32_t  valid:1;         /**< assert to one if the entry is valid                    */
  uint32_t  state:7;         /**< state of the load balancing group: UP/DOWN/DEPENDANCY  */
  uint32_t  valid_prj:1;     /**< assert to 1 if the projection entry is significant     */
  uint32_t  projection_id:7; /**< index of the projection id associated with the entry   */
  uint32_t lbg_id:16;        /**< reference of the load balancing group                  */
  uint16_t sid;              /**< reference of the SID associated with the storage       */
} rozofs_storcli_lbg_prj_assoc_t;


/**
* callback for sending a response to a read or write request
 @param buffer : pointer to the ruc_buffer that cointains the response
 @param socket_ref : index of the scoket context with the caller is remode, non significant for local caller
 @param user_param_p : pointer to a user opaque parameter (non significant for a remote access)
 */
typedef int (*rozofs_storcli_resp_pf_t)(void *buffer,uint32_t socket_ref,void *user_param);


/**
* Macro to get the index of a storage/lbg in a cluster.
  @param idx : index typically the bit idx of a distribution bitmap
  @param base : relative index of the first storage/lbg in the distribution associated with a file
*/
//#define ROZOFS_IDX_IN_CLUSTER(idx,base) ((idx+base)%ROZOFS_SAFE_MAX)
static inline uint8_t ROZOFS_IDX_IN_CLUSTER(uint8_t idx,uint8_t base)
{
  uint8_t data = ((idx+base)%ROZOFS_SAFE_MAX);
  return data;

}
/**
* common context for read and write
*/
typedef struct _rozofs_storcli_ctx_t
{
  ruc_obj_desc_t link;
  uint32_t            index;         /**< Index of the MS */
  uint32_t            free;          /**< Is the context free or allocated TRUE/FALSE */
  uint32_t            integrity;     /**< the value of this field is incremented at  each MS ctx allocation  */
  fid_t               fid_key;       /**< fid value extracted from the read or write request used as a key for the hash table */
  uint32_t            opcode_key;   /**< opcode associated with the request, when the key is not used the value is STORCLI_NULL */
  int       sched_idx;            /**< index within the scheduler table */
  void      *user_param;           /**< pointer to an opaque user param: used for internal read only        */  
  void      *recv_buf;        /**< pointer to the receive buffer that carries the request        */
  uint32_t   socketRef;       /**< reference of the socket on which the answser must be sent     */
  rozofs_storcli_resp_pf_t  response_cbk;  /**< callback function associated with the response of the root transaction */
  uint32_t   src_transaction_id;  /**< transaction id of the source request                                       */
  void      *xmitBuf;             /**< reference of the xmit buffer that will use for sending the response        */
  uint32_t   read_seqnum;         /**< read sequence number that must be found in the reply to correlate with ctx */
  rozofs_storcli_projection_ctx_t  prj_ctx[ROZOFS_SAFE_MAX];
  rozofs_storcli_lbg_prj_assoc_t lbg_assoc_tb[ROZOFS_SAFE_MAX]; /**< association table between lbg and projection */
  rozofs_storcli_inverse_block_t block_ctx_table[ROZOFS_DISTRIBUTION_MAX_SIZE];  

  /*
  ** working variables for read
  */
  uint32_t   read_ctx_lock;      /**< to prevent a dead lock on direct xmit error when received callback is called before returning from the xmit  */
  uint32_t   cur_nmbs2read;      /**< relative index of the first block that must be read           */
  uint32_t   cur_nmbs;           /**< index of the block for the next read                          */
  uint32_t   nb_projections2read; /**< current number of projections with the same distribution     */
  uint32_t   effective_number_of_blocks;  /**< number of blocks that have been effectively read (after transformation) */
  uint8_t    redundancyStorageIdxCur;
  char      *data_read_p;              /**< pointer to the payload of the read data buffer-> output buffer */
  storcli_read_arg_t  storcli_read_arg;           /**< parameter of the read request received from the client (north)         */

  void       *shared_mem_p;  /**< pointer to the shared memory used for reading */
  uint8_t storage_idx_tb[ROZOFS_SAFE_MAX];

  /*
  ** working variables for write
  */
  uint32_t                          empty_wr_block_bitmap; /**< bitmap of the empty blocks */
  uint64_t                          timestamp2;
  uint64_t                          timestamp;
//  void                              *write_rq_p;          /**< pointer to the payload of the write request                       */
  storcli_write_arg_no_data_t       storcli_write_arg;      /**< pointer to the write request arguments                            */
  char                              *data_write_p;          /**< pointer to the payload of the write data buffer->input buffer     */
  uint32_t                           write_ctx_lock;        /**< lock to prevent a release of the write context while sending internal read */
  rozofs_storcli_ingress_write_buf_t wr_proj_buf[ROZOFS_WR_MAX];
//  sproto_write_rsp_t                *write_rsp_p;     /**< pointer to the write response header           */
  uint64_t                          wr_bid;             /**< index of the first block to write              */
  uint32_t                          wr_nb_blocks;       /**< number of blocks to write                      */
  dist_t                            wr_distribution;    /**< distribution for the write                     */
//  uint32_t                          last_block_size;  /**< effective size of the last block: written in the header of the last projection     */
  ruc_obj_desc_t                      timer_list;       /**< timer linked list used as a guard timer upon received first projection             */
  uint8_t      rozofs_storcli_prj_idx_table[ROZOFS_SAFE_MAX*ROZOFS_DISTRIBUTION_MAX_SIZE];  /**< table of the projection used by the inverse process */

  /*
  ** working variables for truncate
  */
  storcli_truncate_arg_t storcli_truncate_arg;  /**< truncate parameter of the request */
  int                    truncate_bins_len;
  /*
  ** working variables for delete
  */
  storcli_delete_arg_t storcli_delete_arg;  /**< delete parameter of the request */

} rozofs_storcli_ctx_t;

/*
** common structure for Mojette transform KPI
*/
typedef struct _storcli_kpi_t
{
   uint64_t  timestamp;
   uint64_t  count;        /**< number of time the function is called */
   uint64_t  elapsed_time;  /**< cumulated  time */
   uint64_t  bytes_count;  /**< cumulated bytes count */
} storcli_kpi_t;



/**
* structure for shared memory management
*/
typedef struct _storcli_shared_t
{
   int active;           /**< assert to 1 if the shared memory is in used */
   key_t key;            /**< key of the shared memory pool */
   uint32_t buf_sz;      /**< size of a buffer              */
   uint32_t buf_count;   /**< number of buffer              */
   void *data_p;         /**< pointer to the beginning of the shared memory     */
   int   error;         /**< errno      */
} storcli_shared_t; 

/*
** reference of the shared memory opened by rozofsmount
*/
extern storcli_shared_t storcli_rozofsmount_shared_mem[];
/**
* Macro associated with KPI
*  @param buffer : kpi buffer
*/
#define STORCLI_START_KPI(buffer)\
 { \
  unsigned long long time;\
  struct timeval     timeDay;  \
  gettimeofday(&timeDay,(struct timezone *)0);  \
  time = MICROLONG(timeDay); \
  buffer.timestamp =time;\
}

#define STORCLI_STOP_KPI(buffer,the_bytes)\
 { \
  unsigned long long timeAfter;\
  struct timeval     timeDay;  \
  buffer.bytes_count += the_bytes;\
  buffer.count ++;\
  gettimeofday(&timeDay,(struct timezone *)0);  \
  timeAfter = MICROLONG(timeDay); \
  buffer.elapsed_time += (timeAfter- buffer.timestamp); \
}
extern storcli_kpi_t storcli_kpi_transform_forward;
extern storcli_kpi_t storcli_kpi_transform_inverse;

#define STORCLI_START_NORTH_PROF(buffer,the_probe, the_bytes)\
 { \
  unsigned long long time;\
  struct timeval     timeDay;  \
  gprofiler.the_probe[P_COUNT]++;\
  gprofiler.the_probe[P_BYTES] += the_bytes;\
  if (buffer != NULL)\
    {\
        gettimeofday(&timeDay,(struct timezone *)0);  \
        time = MICROLONG(timeDay); \
        (buffer)->timestamp =time;\
    }\
}

#define STORCLI_START_NORTH_PROF_SRV(buffer,the_probe, the_bytes)\
 { \
  unsigned long long time;\
  struct timeval     timeDay;  \
  gprofiler.the_probe[P_COUNT]++;\
  gprofiler.the_probe[P_BYTES] += the_bytes;\
  if (buffer != NULL)\
    {\
        gettimeofday(&timeDay,(struct timezone *)0);  \
        time = MICROLONG(timeDay); \
        (buffer)->timestamp2 =time;\
    }\
}
/**
*  Macro METADATA stop non blocking case
*/
#define STORCLI_STOP_NORTH_PROF(buffer,the_probe,the_bytes)\
{ \
  unsigned long long timeAfter;\
  struct timeval     timeDay;  \
  gprofiler.the_probe[P_BYTES] += the_bytes;\
  if (buffer != NULL)\
  { \
    gettimeofday(&timeDay,(struct timezone *)0);  \
    timeAfter = MICROLONG(timeDay); \
    gprofiler.the_probe[P_ELAPSE] += (timeAfter-(buffer)->timestamp); \
  }\
}

#define STORCLI_STOP_NORTH_PROF_SRV(buffer,the_probe,the_bytes)\
{ \
  unsigned long long timeAfter;\
  struct timeval     timeDay;  \
  gprofiler.the_probe[P_BYTES] += the_bytes;\
  if (buffer != NULL)\
  { \
    gettimeofday(&timeDay,(struct timezone *)0);  \
    timeAfter = MICROLONG(timeDay); \
    gprofiler.the_probe[P_ELAPSE] += (timeAfter-(buffer)->timestamp2); \
  }\
}


#define STORCLI_ERR_PROF(the_probe)\
 { \
  gprofiler.the_probe[P_COUNT]++;\
}
/**
* transaction statistics
*/
typedef enum 
{
  ROZOFS_STORCLI_SEND =0 ,
  ROZOFS_STORCLI_SEND_ERROR,
  ROZOFS_STORCLI_RECV_OK, 
  ROZOFS_STORCLI_RECV_OUT_SEQ,
  ROZOFS_STORCLI_TIMEOUT,
  ROZOFS_STORCLI_ENCODING_ERROR,
  ROZOFS_STORCLI_DECODING_ERROR,
  ROZOFS_STORCLI_NO_CTX_ERROR,
  ROZOFS_STORCLI_NO_BUFFER_ERROR,
  ROZOFS_STORCLI_EMPTY_READ,    /**< number of empty blocks read  (read and clear) */
  ROZOFS_STORCLI_EMPTY_WRITE,    /**< number of empty blocks written (read and clear)  */
  ROZOFS_STORCLI_COUNTER_MAX
}rozofs_storcli_tx_stats_e;

extern uint64_t rozofs_storcli_stats[];

#define ROZOFS_STORCLI_STATS(counter) rozofs_storcli_stats[counter]++;

/**
* Buffers information
*/

extern int rozofs_storcli_north_buf_count;
extern int rozofs_storcli_north_small_buf_sz;
extern int rozofs_storcli_north_large_buf_count;
extern int rozofs_storcli_north_large_buf_sz;
extern int rozofs_storcli_south_small_buf_count;
extern int rozofs_storcli_south_small_buf_sz;
extern int rozofs_storcli_south_large_buf_count;
extern int rozofs_storcli_south_large_buf_sz;


extern uint32_t rozofs_storcli_seqnum ;

/**
* Buffer Pools
*/
typedef enum 
{
  _ROZOFS_STORCLI_NORTH_SMALL_POOL =0 ,
  _ROZOFS_STORCLI_NORTH_LARGE_POOL, 
  _ROZOFS_STORCLI_SOUTH_SMALL_POOL,
  _ROZOFS_STORCLI_SOUTH_LARGE_POOL,
  _ROZOFS_STORCLI_MAX_POOL
} rozofs_storcli_buffer_pool_e;

extern void *rozofs_storcli_pool[];


#define ROZOFS_STORCLI_NORTH_SMALL_POOL rozofs_storcli_pool[_ROZOFS_STORCLI_NORTH_SMALL_POOL]
#define ROZOFS_STORCLI_NORTH_LARGE_POOL rozofs_storcli_pool[_ROZOFS_STORCLI_NORTH_LARGE_POOL]
#define ROZOFS_STORCLI_SOUTH_SMALL_POOL rozofs_storcli_pool[_ROZOFS_STORCLI_SOUTH_SMALL_POOL]
#define ROZOFS_STORCLI_SOUTH_LARGE_POOL rozofs_storcli_pool[_ROZOFS_STORCLI_SOUTH_LARGE_POOL]

/*
**_________________________________________________________________________________
*/
/**
* Section concerning the supervision of the lbg connectivity
*/

#define STORCLI_LBG_RUNNING 0       
#define STORCLI_LBG_DOWNGRADED 1    /**< the lbg is in quarantine   */

#define STORCLI_MAX_LBG  256       /**< max number of LBG supported  */
#define STORCLI_LBG_SUP_TMO_THRES1  1
#define STORCLI_LBG_SUP_TMO_MAX     8

#define STORCLI_LBG_BASE_DELAY (20*10)  /**< 20 s */
#define STORCLI_LBG_SP_NULL_INTERVAL (5*10)  /**< 5 s period  */

#define STORCLI_POLL_IDLE  0  /**< no polling towards LBG */
#define STORCLI_POLL_IN_PRG 1 /**< transaction in progress */
#define STORCLI_POLL_ERR 2    /**< sp_null error */

/*
** 
*/
typedef struct _storcli_lbg_cnx_supervision_t
{
//  uint64_t   expiration_date;  /**< date for which the lbg  leaves the quarantine  */
  uint64_t   next_poll_date;   /**< date for the next polling  */
  uint16_t   state:2 ;         /*< state : RUNNING/DOWNGRADED  */
  uint16_t   poll_state:2 ;         /*< polling state  */
  uint16_t   tmo_counter ; 
  uint16_t   poll_counter ; 
} storcli_lbg_cnx_supervision_t;
/*
** the tmo_counter is cleared each time a successful reception happens
**
** The expiration date depends on the tmo_counter
*/


extern storcli_lbg_cnx_supervision_t storcli_lbg_cnx_supervision_tab[];


/**
*  init of the load balancing group supervision table
*/

static inline void storcli_lbg_cnx_sup_init()
{
  int i;
  for (i = 0; i < STORCLI_MAX_LBG;i++)
  {
    storcli_lbg_cnx_supervision_tab[i].state = STORCLI_LBG_RUNNING;
    storcli_lbg_cnx_supervision_tab[i].poll_state = STORCLI_POLL_IDLE;
    storcli_lbg_cnx_supervision_tab[i].tmo_counter = 0;
    storcli_lbg_cnx_supervision_tab[i].poll_counter = 0;
//    storcli_lbg_cnx_supervision_tab[i].expiration_date = 0;  
    storcli_lbg_cnx_supervision_tab[i].next_poll_date = 0;  
  }
}

/**
*  Increment the time-out counter of a load balancing group
  
  @param lbg_id : index of the load balancing group
  
  @retval none
 */
  
static inline void storcli_lbg_cnx_sup_increment_tmo(int lbg_id)
{
 storcli_lbg_cnx_supervision_t *p;
 if (lbg_id >=STORCLI_MAX_LBG) return;
 
 p = &storcli_lbg_cnx_supervision_tab[lbg_id];
 p->tmo_counter++;
// p->expiration_date =  (p->tmo_counter*STORCLI_LBG_BASE_DELAY) + timer_get_ticker();
 p->state = STORCLI_LBG_DOWNGRADED;
}


/**
*  clear the time-out counter of a load balancing group
  
  @param lbg_id : index of the load balancing group
  
  @retval none
 */
  
static inline void storcli_lbg_cnx_sup_clear_tmo(int lbg_id)
{
 storcli_lbg_cnx_supervision_t *p;
 if (lbg_id >=STORCLI_MAX_LBG) return;
 
 p = &storcli_lbg_cnx_supervision_tab[lbg_id];
 p->tmo_counter= 0;
 p->state = STORCLI_LBG_RUNNING;
// p->expiration_date = 0;
}


/**
*  Check if a load balancing group is selectable based on the tmo counter
  @param lbg_id : index of the load balancing group
  
  @retval 0 non selectable
  @retval 1  selectable
 */
  
int storcli_lbg_cnx_sup_is_selectable(int lbg_id);


/*
**_________________________________________________________________________________
*/

/*
*________________________________________________________
*/
/**
  Insert the current request context has the end of its
  associated hash queue.
  That context must be removed from
  that list at the end of the processing of the request
  If there is some pendong request on the same hash queue
  the system must take the first one of the queue and
  activate the processing of that request.
  By construction, the system does not accept more that
  one operation on the same fid (read/write or truncate
  

  @param ctx_p: pointer to the context to insert
   
 
  @retval none
*/
void storcli_hash_table_insert_ctx(rozofs_storcli_ctx_t *ctx_p);


/*
*________________________________________________________
*/
/**
  Search for a call context with the xid as a key

  @param fid: file id to search
   
  @retval <>NULL pointer to searched context
  @retval NULL context is not found
*/
rozofs_storcli_ctx_t *storcli_hash_table_search_ctx(fid_t fid);

/*
**__________________________________________________________________________
*/
/**
  allocate a  context to handle a client read/write transaction

  @param     : none
  @retval <>NULL pointer to the allocated context
  @retval NULL out of free context
*/
rozofs_storcli_ctx_t *rozofs_storcli_alloc_context();

/*
**__________________________________________________________________________
*/
/**
* release a read/write context that has been use for either a read or write operation

  @param : pointer to the context
  
  @retval <>NULL pointer to the allocated context
  @retval NULL out of free context
*/
void rozofs_storcli_release_context(rozofs_storcli_ctx_t *ctx_p);


extern uint32_t    rozofs_storcli_ctx_count;           /**< Max number of contexts    */
extern uint32_t    rozofs_storcli_ctx_allocated;      /**< current number of allocated context        */
/*
**__________________________________________________________________________
*/
/**
*  Get the number of free transaction context
*
  @param none
  
  @retval number of free context
*/
static inline uint32_t rozofs_storcli_get_free_transaction_context()
{
  return(rozofs_storcli_ctx_count - rozofs_storcli_ctx_allocated);

}
/*
**__________________________________________________________________________
*/

/**
* API for creation a transaction towards an exportd

 The reference of the north load balancing is extracted for the client structure
 user_ctx_p:
 That API needs the pointer to the current fuse context. That nformation will be
 saved in the transaction context as userParam. It is intended to be used later when
 the client gets the response from the server
 encoding function;
 For making that API generic, the caller is intended to provide the function that
 will encode the message in XDR format. The source message that is encoded is 
 supposed to be pointed by msg2encode_p.
 Since the service is non-blocking, the caller MUST provide the callback function 
 that will be used for decoding the message
 

 @param lbg_id     : reference of the load balancing group
 @param timeout_sec : transaction timeout
 @param prog       : program
 @param vers       : program version
 @param opcode     : metadata opcode
 @param encode_fct : encoding function
 @msg2encode_p     : pointer to the message to encode
 @param xmit_buf : pointer to the buffer to send, in case of error that function release the buffer
 @param seqnum     : sequence number associated with the context (store as an opaque parameter in the transaction context
 @param opaque_value_idx1 : opaque value at index 1
 @param extra_len  : extra length to add after encoding RPC (must be 4 bytes aligned !!!)
 @param recv_cbk   : receive callback function

 @param user_ctx_p : pointer to the working context
 
 @retval 0 on success;
 @retval -1 on error,, errno contains the cause
 */

#if 1
int rozofs_sorcli_send_rq_common(uint32_t lbg_id,uint32_t timeout_sec, uint32_t prog,uint32_t vers,
                              int opcode,xdrproc_t encode_fct,void *msg2encode_p,
                              void *xmit_buf,
                              uint32_t seqnum,
                              uint32_t opaque_value_idx1,  
                              int      extra_len,                            
                              sys_recv_pf_t recv_cbk,void *user_ctx_p);

#endif


/*
**__________________________________________________________________________
*/
/**
* send a success read reply
  That API fill up the common header with the SP_READ_RSP opcode
  insert the transaction_id associated with the inittial request transaction id
  insert a status OK
  insert the length of the data payload
  
  In case of a success it is up to the called function to release the xmit buffer
  
  @param p : pointer to the root transaction context used for the read
  
  @retval none

*/
void rozofs_storcli_read_reply_success(rozofs_storcli_ctx_t *p);

/*
**__________________________________________________________________________
*/
/**
* send a success read reply
  That API fill up the common header with the SP_READ_RSP opcode
  insert the transaction_id associated with the inittial request transaction id
  insert a status OK
  insert the length of the data payload
  
  In case of a success it is up to the called function to release the xmit buffer
  
  @param p : pointer to the root transaction context used for the read
  @param error : error code
  
  @retval none

*/
void rozofs_storcli_read_reply_error(rozofs_storcli_ctx_t *p,int error);
/*
**____________________________________________________
*/
/*
  start a periodic timer to chech wether the export LBG is down
  When the export is restarted its port may change, and so
  the previous configuration of the LBG is not valid any more
*/
void rozofs_storcli_read_init_timer_module();

/*
**____________________________________________________
*/
/**
* stop the read guard timer

  @param p: read main context
  
 @retval none
*/
void rozofs_storcli_stop_read_guard_timer(rozofs_storcli_ctx_t  *p);
/*
**____________________________________________________
*/
/**
* start the read guard timer: must be called upon the reception of the first projection

  @param p: read main context
  
 @retval none
*/
void rozofs_storcli_start_read_guard_timer(rozofs_storcli_ctx_t  *p);
/*
**__________________________________________________________________________
*/
/**
* send a successfull write reply
  That API fill up the common header with the SP_READ_RSP opcode
  insert the transaction_id associated with the inittial request transaction id
  insert a status OK
  insert the length of the data payload
  
  In case of a success it is up to the called function to release the xmit buffer
  
  @param p : pointer to the root transaction context used for the read
  
  @retval none

*/
void rozofs_storcli_write_reply_success(rozofs_storcli_ctx_t *p);

/*
**__________________________________________________________________________
*/
/**
* send a successfull delete reply
  That API fill up the common header with the SP_READ_RSP opcode
  insert the transaction_id associated with the inittial request transaction id
  insert a status OK
  insert the length of the data payload
  
  In case of a success it is up to the called function to release the xmit buffer
  
  @param p : pointer to the root transaction context used for the read
  
  @retval none

*/
void rozofs_storcli_delete_reply_success(rozofs_storcli_ctx_t *p);
/*
**__________________________________________________________________________
*/
/**
* send a success read reply
  That API fill up the common header with the SP_READ_RSP opcode
  insert the transaction_id associated with the inittial request transaction id
  insert a status OK
  insert the length of the data payload
  
  In case of a success it is up to the called function to release the xmit buffer
  
  @param p : pointer to the root transaction context used for the read
  @param error : error code
  
  @retval none

*/
void rozofs_storcli_write_reply_error(rozofs_storcli_ctx_t *p,int error);

/*
**__________________________________________________________________________
*/
/**
* send a error read reply by using the receiver buffer
 
  @param socket_ctx_idx: index of the TCP connection
  @param recv_buf: pointer to the ruc_buffer that contains the message
  @param rozofs_storcli_remote_rsp_cbk: callback for sending out the response
  @param user_param : pointer to a user opaque parameter (non significant for a remote access)
  @param error : error code
  
  @retval none

*/
void rozofs_storcli_reply_error_with_recv_buf(uint32_t  socket_ctx_idx,
                                              void *recv_buf,
                                              void *user_param,
                                              rozofs_storcli_resp_pf_t rozofs_storcli_remote_rsp_cbk,
                                              int error);


/*
**__________________________________________________________________________
*/
/**
  Initial read request
    
  @param socket_ctx_idx: index of the TCP connection
  @param recv_buf: pointer to the ruc_buffer that contains the message
  @param rozofs_storcli_remote_rsp_cbk: callback for sending out the response
  @param user_param : pointer to a user opaque parameter (non significant for a remote access)
  @param do_not_queue: when asserted, the request in not inserted in the serialization hash table
 
   @retval : TRUE-> xmit ready event expected
  @retval : FALSE-> xmit  ready event not expected
*/
void rozofs_storcli_read_req_init(uint32_t  socket_ctx_idx, 
                                  void *recv_buf,
                                  rozofs_storcli_resp_pf_t rozofs_storcli_remote_rsp_cbk,
                                  void *user_param,
                                  uint32_t do_not_queue);

int rozofs_storcli_remote_rsp_cbk(void *buffer,uint32_t socket_ref,void *user_param);
/*
**__________________________________________________________________________
*/

void rozofs_storcli_read_req_processing(rozofs_storcli_ctx_t *working_ctx_p);
/*
**__________________________________________________________________________
*/
/**
  Initial write request


    
  @param socket_ctx_p: pointer to the af unix socket
  @param socketId: reference of the socket (not used)
  @param rozofs_storcli_remote_rsp_cbk: callback for sending out the response
 
   @retval : TRUE-> xmit ready event expected
  @retval : FALSE-> xmit  ready event not expected
*/
void rozofs_storcli_write_req_init(uint32_t  socket_ctx_idx, 
                                   void*recv_buf,
                                   rozofs_storcli_resp_pf_t rozofs_storcli_remote_rsp_cbk);

/*
**__________________________________________________________________________
*/
/**
  Prepare to execute a write request: 
   that function is called either directly from from rozofs_storcli_write_req_init() if
   there is request with the same fid that is currently processed,
   or at the end of the processing of a request with the same fid (from rozofs_storcli_release_context()).
    
 @param working_ctx_p: pointer to the root context associated with the top level write request

 
   @retval : none
*/

void rozofs_storcli_write_req_processing_exec(rozofs_storcli_ctx_t *working_ctx_p);

/*
**__________________________________________________________________________
*/
/**
  Initial truncate request
    
  @param socket_ctx_p: pointer to the af unix socket
  @param socketId: reference of the socket (not used)
  @param rozofs_storcli_remote_rsp_cbk: callback for sending out the response
 
   @retval : TRUE-> xmit ready event expected
  @retval : FALSE-> xmit  ready event not expected
*/
void rozofs_storcli_truncate_req_init(uint32_t  socket_ctx_idx, void *recv_buf,rozofs_storcli_resp_pf_t rozofs_storcli_remote_rsp_cbk);

/*
**__________________________________________________________________________
*/
/*
** That function is called when all the projection are ready to be sent

 @param working_ctx_p: pointer to the root context associated with the top level write request

*/
void rozofs_storcli_truncate_req_processing(rozofs_storcli_ctx_t *working_ctx_p);
/*
**__________________________________________________________________________
*/
/**
  Initial delete request
    
  @param socket_ctx_p: pointer to the af unix socket
  @param socketId: reference of the socket (not used)
  @param rozofs_storcli_remote_rsp_cbk: callback for sending out the response
 
   @retval : TRUE-> xmit ready event expected
  @retval : FALSE-> xmit  ready event not expected
*/
void rozofs_storcli_delete_req_init(uint32_t  socket_ctx_idx, void *recv_buf,rozofs_storcli_resp_pf_t rozofs_storcli_remote_rsp_cbk);
/*
**__________________________________________________________________________
*/
/*
** That function is called when all the projection are ready to be sent

 @param working_ctx_p: pointer to the root context associated with the top level write request

*/
void rozofs_storcli_delete_req_processing(rozofs_storcli_ctx_t *working_ctx_p);
/*
**__________________________________________________________________________
*/
/**
*  allocate a sequence number of a new context
*/
static inline uint32_t rozofs_storcli_allocate_read_seqnum()
{
  rozofs_storcli_seqnum++;
  if (rozofs_storcli_seqnum == 0) rozofs_storcli_seqnum = 1;
  return rozofs_storcli_seqnum;
}


/*
**__________________________________________________________________________
*/
/**
*  clear the projection/lbg_id association table
 
   @param lbg_assoc_p: pointer to the beginning of the association table
*/
static inline void rozofs_storcli_lbg_prj_clear(rozofs_storcli_lbg_prj_assoc_t *lbg_assoc_p)
{
  memset(lbg_assoc_p,0,sizeof(rozofs_storcli_lbg_prj_assoc_t)*ROZOFS_SAFE_MAX);
}

/*
**__________________________________________________________________________
*/
/**
*  set the reference of the lbg_id in the projection/lbg association table
 
   @param lbg_assoc_p: pointer to the beginning of the association table
   @param index: index of the entry in the association table
   @param lbg_id: index of the load balancing group
   @param sid : reference of the storage

*/
static inline void rozofs_storcli_lbg_prj_insert_lbg_and_sid(rozofs_storcli_lbg_prj_assoc_t *lbg_assoc_p,int index,int lbg_id,int sid)
{
  lbg_assoc_p[index].valid  = 1;
  lbg_assoc_p[index].lbg_id = lbg_id;
  lbg_assoc_p[index].sid    = sid;
}



/*
**__________________________________________________________________________
*/
/**
*  get the reference of the lbg_id in the projection/lbg association table
 
   @param lbg_assoc_p: pointer to the beginning of the association table
   @param index: index of the entry in the association table
   
   @retval lbg_id: index of the load balancing group
   @retval -1: invalid inout index
*/
static inline int rozofs_storcli_lbg_prj_get_lbg(rozofs_storcli_lbg_prj_assoc_t *lbg_assoc_p,int index)
{
  if (lbg_assoc_p[index].valid  == 1) return (int)lbg_assoc_p[index].lbg_id;
  return -1;
}


/*
**__________________________________________________________________________
*/
/**
*  get the reference of the sid in the projection/lbg association table
 
   @param lbg_assoc_p: pointer to the beginning of the association table
   @param index: index of the entry in the association table
   
   @retval lbg_id: index of the load balancing group
   @retval -1: invalid inout index
*/
static inline int rozofs_storcli_lbg_prj_get_sid(rozofs_storcli_lbg_prj_assoc_t *lbg_assoc_p,int index)
{
  if (lbg_assoc_p[index].valid  == 1) return (int)lbg_assoc_p[index].sid;
  return -1;
}



/*
**__________________________________________________________________________
*/
/**
*  set the state of the lbg_id in the projection/lbg association table
   Here it is assume that the reference of the load balancing group has already been set
   
   @param lbg_assoc_p: pointer to the beginning of the association table
   @param index: index of the entry in the association table
   @param state: state of the load balancing group

*/
static inline void rozofs_storcli_lbg_prj_insert_lbg_state(rozofs_storcli_lbg_prj_assoc_t *lbg_assoc_p,int index,int state)
{
  if (lbg_assoc_p[index].valid == 0)
  {
    lbg_assoc_p[index].state = NORTH_LBG_DEPENDENCY;
  }
  else
  {
    lbg_assoc_p[index].state  = state;
  }
}

/*
**__________________________________________________________________________
*/
/**
*  Get the state of the lbg_id in the projection/lbg association table
   Here it is assume that the reference of the load balancing group has already been set
   
   @param lbg_assoc_p: pointer to the beginning of the association table
   @param index: index of the entry in the association table
   
   @retval state: state of the load balancing group
*/
static inline int rozofs_storcli_lbg_prj_get_lbg_state(rozofs_storcli_lbg_prj_assoc_t *lbg_assoc_p,int index)
{
  if (lbg_assoc_p[index].valid == 0)
  {
    return NORTH_LBG_DEPENDENCY;
  }
  return lbg_assoc_p[index].state ;
}

/*
**__________________________________________________________________________
*/
/**
*  Function to check if the load balancing group is selectable.

   The load balancing group is selectable if is state is UP and there is no associated with a projection id
   
   @param lbg_assoc_p: pointer to the beginning of the association table
   @param index: index of the entry in the association table
   @param down_acceptable: set to 1 is DOWN state is acceptable
   
   @retval 1:  selectable
   @retval 0: not selectable
*/
static inline int rozofs_storcli_lbg_prj_is_lbg_selectable(rozofs_storcli_lbg_prj_assoc_t *lbg_assoc_p,int index,int down_acceptable)
{
  if (lbg_assoc_p[index].valid == 0)
  {
    return 0;
  }
  if (lbg_assoc_p[index].valid_prj )
  {
    return 0;  
  }
  /*
  ** check if the lbg is not in quarantine
  */
  if (north_lbg_is_available(lbg_assoc_p[index].lbg_id) == 0) return 0;
  
  if (storcli_lbg_cnx_sup_is_selectable(lbg_assoc_p[index].lbg_id) == 0) return 0;
  
  if (lbg_assoc_p[index].state == NORTH_LBG_UP)
  {
    return 1;
  }
  if ((lbg_assoc_p[index].state == NORTH_LBG_DOWN) && (down_acceptable))
  {
    return 1;
  }
  return 0;
}

/*
**__________________________________________________________________________
*/
/**
*  Function to set the reference of a projection.

   it is assumed that the load balancing group index has already been inserted
   
   @param lbg_assoc_p: pointer to the beginning of the association table
   @param index: index of the entry in the association table
   @param projection_id: index of the projection
   
   @retval none
*/
static inline void rozofs_storcli_lbg_prj_set_projection_id(rozofs_storcli_lbg_prj_assoc_t *lbg_assoc_p,int index,uint8_t projection_id)
{

  lbg_assoc_p[index].valid_prj = 1; 
  lbg_assoc_p[index].projection_id = 1; 
}


/*
**__________________________________________________________________________
*/
/**
*  Function to set the reference of a projection.

   it is assumed that the load balancing group index has already been inserted
   
   @param lbg_assoc_p: pointer to the beginning of the association table
   @param index: index of the entry in the association table
   @param projection_id: index of the projection
   
   @retval none
*/
static inline void rozofs_storcli_lbg_prj_clear_projection_id(rozofs_storcli_lbg_prj_assoc_t *lbg_assoc_p,int index,uint8_t projection_id)
{
  lbg_assoc_p[index].valid_prj = 0; 
}


/*
**__________________________________________________________________________
*/
/**
*  Attempt to select a storage for a given projection index

  @param working_ctx_p: pointer to the root transaction context (read or write)
  @param rozofs_safe: number of storage for the layout
  @param projection_id: local index of the projection for which a storage is neeeded
  
  @retval 1 : a storage has been and the projection context has been updated with the storage idx
  @retval < 0 : no storage found
*/
static inline int rozofs_storcli_select_storage_idx (rozofs_storcli_ctx_t *working_ctx_p, uint8_t rozofs_safe, uint8_t projection_id)
{
   int storage_idx = 0;
   rozofs_storcli_projection_ctx_t *prj_cxt_p   = working_ctx_p->prj_ctx;   

   /*
   ** find the host that has this projection (projection_id) among the 
   ** rozofs_safe set
   */
   int lbg_down_acceptable = 0;
   while(1)
   {
     for (storage_idx = 0; storage_idx < rozofs_safe; storage_idx++)
     {
       if (rozofs_storcli_lbg_prj_is_lbg_selectable(working_ctx_p->lbg_assoc_tb,storage_idx,lbg_down_acceptable))
       {
         /*
         ** The lbg is up: store the index in the projection context
         */
          prj_cxt_p[projection_id].valid_stor_idx = 1;
          prj_cxt_p[projection_id].stor_idx       = storage_idx; 
          /*
          ** store the reference of the projection in the association table
          */
          rozofs_storcli_lbg_prj_set_projection_id(working_ctx_p->lbg_assoc_tb,storage_idx,projection_id);
          return 1;
          break;              
       }
     }
     /*
     ** include a DOWN LBG in the selection list. If it was already done we are in trouble since
     ** there is no enough storage for reading the data (no enough projections for rebuilding
     */
     if (lbg_down_acceptable == 1) return -1;
     /*
     ** OK, give a change with a storage that has a down connection (assume that it is just a temporary
     ** failure of the TCP connection
     */
     lbg_down_acceptable = 1;
   }
   return -1;
}



/*
**__________________________________________________________________________
*/
/**
*  Attempt to select a storage for a given projection index for the write case

  The system attempts to select the storage that has the same index as the projection. If it cannot do it 

  @param working_ctx_p: pointer to the root transaction context (read or write)
  @param rozofs_forward: minimal number of storage for write
  @param rozofs_safe: number of storage for the layout
  @param projection_id: local index of the projection for which a storage is neeeded
  
  @retval 1 : a storage has been and the projection context has been updated with the storage idx
  @retval < 0 : no storage found
*/
static inline int rozofs_storcli_select_storage_idx_for_write (rozofs_storcli_ctx_t *working_ctx_p, 
                                                               uint8_t rozofs_forward,uint8_t rozofs_safe, uint8_t projection_id)
{
   int storage_idx = 0;
   rozofs_storcli_projection_ctx_t *prj_cxt_p   = working_ctx_p->prj_ctx;   
   int lbg_down_acceptable = 0;
   
   /*
   ** Check if the storage with the same relative projection index is selectable
   */
   if (rozofs_storcli_lbg_prj_is_lbg_selectable(working_ctx_p->lbg_assoc_tb,projection_id,lbg_down_acceptable))
   {
      prj_cxt_p[projection_id].valid_stor_idx = 1;
      prj_cxt_p[projection_id].stor_idx       = projection_id;    
      rozofs_storcli_lbg_prj_set_projection_id(working_ctx_p->lbg_assoc_tb,projection_id,projection_id);

      return 1;   
   }

   /*
   ** The storage that must be associated with the projection in the optimal case is not selectable
   ** attempt to select a storage in the spare storages set
   */
   while(1)
   {
     for (storage_idx = rozofs_forward; storage_idx < rozofs_safe; storage_idx++)
     {
       if (rozofs_storcli_lbg_prj_is_lbg_selectable(working_ctx_p->lbg_assoc_tb,storage_idx,lbg_down_acceptable))
       {
         /*
         ** The lbg is up: store the index in the projection context
         */
          prj_cxt_p[projection_id].valid_stor_idx = 1;
          prj_cxt_p[projection_id].stor_idx       = storage_idx; 
          /*
          ** store the reference of the projection in the association table
          */
          rozofs_storcli_lbg_prj_set_projection_id(working_ctx_p->lbg_assoc_tb,storage_idx,projection_id);
          return 1;
          break;              
       }
     }
     /*
     ** include a DOWN LBG in the selection list. If it was already done we are in trouble since
     ** there is no enough storage for reading the data (no enough projections for rebuilding
     */
     if (lbg_down_acceptable == 1) return -1;
     /*
     ** OK, give a change with a storage that has a down connection (assume that it is just a temporary
     ** failure of the TCP connection
     ** since there is no spare storage in the up state, first attempt with the optimal storage index
     ** by attempting to select it with the down state
     */
     lbg_down_acceptable = 1;
     if (rozofs_storcli_lbg_prj_is_lbg_selectable(working_ctx_p->lbg_assoc_tb,projection_id,lbg_down_acceptable))
     {
        prj_cxt_p[projection_id].valid_stor_idx = 1;
        prj_cxt_p[projection_id].stor_idx       = projection_id;    
        rozofs_storcli_lbg_prj_set_projection_id(working_ctx_p->lbg_assoc_tb,projection_id,projection_id);

        return 1;   
     } 
     /*
     ** the optimal storage seems out of service, so attempt to select a spare storage in the down state
     */    
   }
   return -1;
}

/*
**__________________________________________________________________________
*/
/**
*   Update the state of the load balancing group for a rozofs_safe range

  @param working_ctx_p: pointer to the root transaction context (read or write)
  @param rozofs_safe: number of storage for the layout
  
  @retval none
  
*/
static inline void rozofs_storcli_update_lbg_for_safe_range(rozofs_storcli_ctx_t *working_ctx_p,uint8_t rozofs_safe)
{
  int i;
  rozofs_storcli_lbg_prj_assoc_t  *lbg_assoc_p = working_ctx_p->lbg_assoc_tb;

  for (i = 0; i < rozofs_safe; i ++)
  {
    rozofs_storcli_lbg_prj_insert_lbg_state(lbg_assoc_p,
                                            i,
                                            NORTH_LBG_GET_STATE(lbg_assoc_p[i].lbg_id));    
  }
}


/*
**____________________________________________________
*/
/**
*  
  Applicative Polling of a TCP connection towards Storage
  @param sock_p : pointer to the connection context
  
  @retval none
 */
  
void storcli_lbg_cnx_polling(af_unix_ctx_generic_t  *sock_p);

/*
**__________________________________________________________________________
*/
/*
** That function is called when all the projection are ready to be sent

 @param working_ctx_p: pointer to the root context associated with the top level write request

*/
void rozofs_storcli_write_req_processing(rozofs_storcli_ctx_t *working_ctx_p);

#endif
