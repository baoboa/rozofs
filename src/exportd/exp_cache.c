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
#include <sys/param.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <uuid/uuid.h>
#include <errno.h>
#include <string.h>
 
//#include <rozofs/common/xmalloc.h>
//#include <rozofs/common/profile.h>
//#include <rozofs/rpc/epproto.h>
#include "cache.h"
#include "export.h"
#include <rozofs/common/export_track.h>

#define EXP_MAX_FAKE_LVL2_ENTRIES 16
#define LV2_MAX_ENTRIES (256*1024)
#define LV2_BUKETS 1024

lv2_entry_t  exp_fake_lv2_entry[EXP_MAX_FAKE_LVL2_ENTRIES];
int exp_fake_lv2_entry_idx = 0;



/**
 * hashing function used to find lv2 entry in the cache
 */
static inline uint32_t lv2_hash(void *key) {
    uint32_t hash = 0;
    uint8_t *c;

    for (c = key; c != key + 16; c++)
        hash = *c + (hash << 6) + (hash << 16) - hash;
    return hash;
}

static inline int lv2_cmp(void *k1, void *k2) {
    return uuid_compare(k1, k2);
}
/*
**__________________________________________________________________
*/
/**
*   Remove an entry from the attribute cache

    @param: pointer to the cache context
    @param: pointer to entry to remove
    
    @retval none
*/
static inline void lv2_cache_unlink(lv2_cache_t *cache,lv2_entry_t *entry) {

  file_lock_remove_fid_locks(&entry->file_lock);
  mattr_release(&entry->attributes.s.attrs);
  /*
  ** check the presence of the extended attribute block and free it
  */
  if (entry->extended_attr_p != NULL) free(entry->extended_attr_p);
  /*
  ** check the presence of the root_idx bitmap : for directory only
  */
  if (entry->dirent_root_idx_p != NULL) free(entry->dirent_root_idx_p);  
  list_remove(&entry->list);
  free(entry);
  cache->size--;  
}
/*
**__________________________________________________________________
*/
/**
*   init of an exportd attribute cache

    @param: pointer to the cache context
    
    @retval none
*/
void lv2_cache_initialize(lv2_cache_t *cache) {
    cache->max = LV2_MAX_ENTRIES;
    cache->size = 0;
    cache->hit  = 0;
    cache->miss = 0;
    cache->lru_del = 0;
    list_init(&cache->entries);
    htable_initialize(&cache->htable, LV2_BUKETS, lv2_hash, lv2_cmp);
    
    /* 
    ** Lock service initalize 
    */
    file_lock_service_init();
}
/*
**__________________________________________________________________
*/
/**
*   delete of an exportd attribute cache

    @param: pointer to the cache context
    
    @retval none
*/
void lv2_cache_release(lv2_cache_t *cache) {
    list_t *p, *q;

    list_for_each_forward_safe(p, q, &cache->entries) {
        lv2_entry_t *entry = list_entry(p, lv2_entry_t, list);
        htable_del(&cache->htable, entry->attributes.s.attrs.fid);
	lv2_cache_unlink(cache,entry);
    }
}
/*
**__________________________________________________________________
*/
/**
*   Get an enry from the attributes cache

    @param: pointer to the cache context
    @param: fid : key of the element to find
    
    @retval <>NULL : pointer to the cache entry that contains the attributes
    @retval NULL: not found
*/
lv2_entry_t *lv2_cache_get(lv2_cache_t *cache, fid_t fid) {
    lv2_entry_t *entry = 0;

//    START_PROFILING(lv2_cache_get);

    if ((entry = htable_get(&cache->htable, fid)) != 0) {
        // push the lru
        list_remove(&entry->list);
        list_push_front(&cache->entries, &entry->list);
	cache->hit++;
    }
    else {
      cache->miss++;
    }

//    STOP_PROFILING(lv2_cache_get);
    return entry;
}
/*
**__________________________________________________________________
*/
/**
   read the attributes from disk
  
   attributes can be the one of a regular file, symbolic link or a directory.
   The type of the object is indicated within the lower part of the fid (field key)
   
   @param trk_tb_p: export attributes tracking table
   @param fid: unique file identifier
   @param entry : pointer to the array where attributes will be returned
   
   @retval 0 on success
   @retval -1 on error (see errno for details
*/
int exp_meta_get_object_attributes(export_tracking_table_t *trk_tb_p,fid_t fid,lv2_entry_t *entry_p)
{
   int ret;
   rozofs_inode_t *fake_inode;
   exp_trck_top_header_t *p = NULL;
   
   fake_inode = (rozofs_inode_t*)fid;
   
   if (fake_inode->s.key >= ROZOFS_MAXATTR)
   {
     errno = EINVAL;
     return -1;
   }
   p = trk_tb_p->tracking_table[fake_inode->s.key];
   if (p == NULL)
   {
     errno = ENOTSUP;
     return -1;    
   }   
   /*
   ** read the attributes from disk
   */
   ret = exp_metadata_read_attributes(p,fake_inode,entry_p,sizeof(ext_mattr_t));
   if (ret < 0)
   { 
     return -1;
   }  
   return 0; 
}

/*
**__________________________________________________________________
*/
/**
   read the extended attributes block from disk
  
   attributes can be the one of a regular file, symbolic link or a directory.
   The type of the object is indicated within the lower part of the fid (field key)
   
   @param trk_tb_p: export attributes tracking table
   @param entry : pointer to the array where attributes will be returned
   
   @retval 0 on success
   @retval -1 on error (see errno for details
*/
int exp_meta_get_xattr_block(export_tracking_table_t *trk_tb_p,lv2_entry_t *entry_p)
{
   int ret;
   rozofs_inode_t fake_inode;
   exp_trck_top_header_t *p = NULL;
   
   fake_inode.fid[1] = entry_p->attributes.s.i_file_acl;
   
   if (fake_inode.s.key >= ROZOFS_MAXATTR)
   {
     errno = EINVAL;
     return -1;
   }
   p = trk_tb_p->tracking_table[fake_inode.s.key];
   if (p == NULL)
   {
     errno = ENOTSUP;
     return -1;    
   }   
   /*
   ** read the attributes from disk
   */
   ret = exp_metadata_read_attributes(p,&fake_inode,entry_p->extended_attr_p,ROZOFS_XATTR_BLOCK_SZ);
   if (ret < 0)
   { 
     return -1;
   }  
   return 0; 
}
/*
**__________________________________________________________________
*/
/**
*   The purpose of that service is to read object attributes and store them in the attributes cache

  @param trk_tb_p: export attributes tracking table
  @param cache : pointer to the export attributes cache
  @param fid : unique identifier of the object
  
  @retval <> NULL: attributes of the object
  @retval == NULL : no attribute returned for the object (see errno for details)
*/

lv2_entry_t *lv2_cache_put(export_tracking_table_t *trk_tb_p,lv2_cache_t *cache, fid_t fid) {
    lv2_entry_t *entry;

//    START_PROFILING(lv2_cache_put);

    // maybe already cached.
    if ((entry = htable_get(&cache->htable, fid)) != 0) {
        goto out;
    }
    entry = malloc(sizeof(lv2_entry_t));
    if (entry == NULL)
    {
       severe("lv2_cache_put: %s\b",strerror(errno));
       return NULL;
    }
    memset(entry,0,sizeof(lv2_entry_t));

    /*
    ** get the attributes of the object
    */
    if (exp_meta_get_object_attributes(trk_tb_p,fid,entry) < 0)
    {
      /*
      ** cannot get the attributes: need to log the returned errno
      */
      goto error;
    }    
    /*
    ** Initialize file locking 
    */
    list_init(&entry->file_lock);
    entry->nb_locks = 0;
    /*
    ** insert the entry at the top of the cache
    */
    list_push_front(&cache->entries, &entry->list);
    htable_put(&cache->htable, entry->attributes.s.attrs.fid, entry);
    
    if (cache->size++ >= cache->max) { // remove the lru
        lv2_entry_t *lru;
	
        /*
	** Do not remove entries with a file lock set
	*/
	while (1) {
	  lru = list_entry(cache->entries.prev, lv2_entry_t, list);
	  if (lru->nb_locks == 0) break; /* This guy can be removed */
	  /* This guy should be put at the beginning of the list */
          list_push_front(&cache->entries, &lru->list);	  
	}
        htable_del(&cache->htable, lru->attributes.s.attrs.fid);
	lv2_cache_unlink(cache,lru);
	cache->lru_del++;
    }

    goto out;
error:
    if (entry) {
        free(entry);
        entry = 0;
    }
out:
//    STOP_PROFILING(lv2_cache_put);
    return entry;
}
/*
**__________________________________________________________________
*/
/**
*   delete an entry from the attribute cache

    @param cache: pointer to the level 2 cache
    @param fid : key of the entry to remove
*/
void lv2_cache_del(lv2_cache_t *cache, fid_t fid) 
{
    lv2_entry_t *entry = 0;
//    START_PROFILING(lv2_cache_del);

    if ((entry = htable_del(&cache->htable, fid)) != 0) {
	lv2_cache_unlink(cache,entry);
    }
//    STOP_PROFILING(lv2_cache_del);
}

/*
**__________________________________________________________________
*/
/** search a fid in the attribute cache
 
 if fid is not cached, try to find it on the underlying file system
 and cache it.
 
  @param trk_tb_p: export attributes tracking table
  @param cache: pointer to the cache associated with the export
  @param fid: the searched fid
 
  @return a pointer to lv2 entry or null on error (errno is set)
*/
lv2_entry_t *export_lookup_fid(export_tracking_table_t *trk_tb_p,lv2_cache_t *cache, fid_t fid) {
    lv2_entry_t *lv2 = 0;
    uint32_t slice;
//    START_PROFILING(export_lookup_fid);
    
    /*
    ** get the slice of the fid :extracted from the upper part 
    */
    exp_trck_get_slice(fid,&slice);
    /*
    ** check if the slice is local
    */
    if (exp_trck_is_local_slice(slice))
    {
      if (!(lv2 = lv2_cache_get(cache, fid))) {
          // not cached, find it an cache it
          if (!(lv2 = lv2_cache_put(trk_tb_p,cache, fid))) {
              goto out;
          }
      }
    }
    else
    {
      int idx  = (++exp_fake_lv2_entry_idx)%(EXP_MAX_FAKE_LVL2_ENTRIES);
      lv2 = &exp_fake_lv2_entry[idx];
      if (exp_meta_get_object_attributes(trk_tb_p,fid,lv2) < 0)
      {
	/*
	** cannot get the attributes: need to log the returned errno
	*/
	lv2 = NULL;
      } 
      else
      {
         memset(lv2,0,sizeof(lv2_entry_t));
      }         
    }
out:
//    STOP_PROFILING(export_lookup_fid);
    return lv2;
}

/*
**__________________________________________________________________
*/
/** store the attributes part of an attribute cache entry  to the export's file system
 *
   @param trk_tb_p: export attributes tracking table
   @param entry: the entry used
 
   @return: 0 on success otherwise -1
 */
int export_lv2_write_attributes(export_tracking_table_t *trk_tb_p,lv2_entry_t *entry) {

   int ret;
   rozofs_inode_t *fake_inode;
   exp_trck_top_header_t *p = NULL;
   
   fake_inode = (rozofs_inode_t*)entry->attributes.s.attrs.fid;
   if (fake_inode->s.key >= ROZOFS_MAXATTR)
   {
     errno = EINVAL;
     return -1;
   }
   p = trk_tb_p->tracking_table[fake_inode->s.key];
   if (p == NULL)
   {
     errno = ENOTSUP;
     return -1;    
   }
   /*
   ** read the attributes from disk
   */
   ret = exp_metadata_write_attributes(p,fake_inode,entry,sizeof(ext_mattr_t));
   if (ret < 0)
   { 
     return -1;
   }  
   return 0; 
}
/*
**__________________________________________________________________
*/
/**
*    delete an inode associated with an object

   @param trk_tb_p: export attributes tracking table
   @param fid: fid of the object (key)
   
   @retval 0 on success
   @retval -1 on error
*/
int exp_attr_delete(export_tracking_table_t *trk_tb_p,fid_t fid)
{
   rozofs_inode_t *fake_inode;
   exp_trck_top_header_t *p = NULL;
   int ret;

   fake_inode = (rozofs_inode_t*)fid;
   if (fake_inode->s.key >= ROZOFS_MAXATTR)
   {
     errno = EINVAL;
     return -1;
   }
   p = trk_tb_p->tracking_table[fake_inode->s.key];
   if (p == NULL)
   {
     errno = ENOTSUP;
     return -1;    
   }   
   /*
   ** release the inode
   */
   ret = exp_metadata_release_inode(p,fake_inode);
   if (ret < 0)
   { 
      return -1;
   }
   return 0;
}

/*
**__________________________________________________________________
*/
/**
*  Create the attributes of a directory/regular file or symbolic link

  create an oject according to its type. The service performs the allocation of the fid. 
  It is assumed that all the other fields of the object attributes are already been filled in.
  
  @param trk_tb_p: export attributes tracking table
  @param slice: slice of the parent directory
  @param global_attr_p : pointer to the attributes of the object
  @param type: type of the object (ROZOFS_REG: regular file, ROZOFS_SLNK: symbolic link, ROZOFS_DIR : directory
  @param link: pointer to the symbolic link (significant for ROZOFS_SLNK only)
  
  @retval 0 on success: (the attributes contains the lower part of the fid that is allocated by the service)
  @retval -1 on error (see errno for details)
*/
int exp_attr_create(export_tracking_table_t *trk_tb_p,uint32_t slice,ext_mattr_t *global_attr_p,int type,char *link)
{
   fid_t fid;
   rozofs_inode_t *fake_inode;
   rozofs_inode_t fake_inode_link;
   int ret;
   exp_trck_top_header_t *p = NULL;
   exp_trck_top_header_t *p_link = NULL;
   uint32_t link_slice;
   int inode_allocated = 0;
   int link_allocated = 0;
   
   uuid_generate(fid);


   fake_inode = (rozofs_inode_t*)fid;
   fake_inode->fid[1] = 0;
   fake_inode->s.key = type;
   fake_inode->s.usr_id = slice; /** always the parent slice for storage */
   fake_inode->s.eid = trk_tb_p->eid; 
   
   info("eid is %d",  trk_tb_p->eid);
   
   if (fake_inode->s.key >= ROZOFS_MAXATTR)
   {
     errno = EINVAL;
     return -1;
   }
   p = trk_tb_p->tracking_table[fake_inode->s.key];
   if (p == NULL)
   {
     errno = ENOTSUP;
     goto error;
   }   
   /*
   ** allocate the inode
   */
   ret = exp_metadata_allocate_inode(p,fake_inode,type,slice);
   if (ret < 0)
   { 
      goto error;
   }
   inode_allocated = 1;
   /*
   ** copy the definitive fid of the object
   */
   memcpy(&global_attr_p->s.attrs.fid,fake_inode,sizeof(fid_t));
   if (link == NULL)
   {
     /*
     ** write the metadata on disk for the directory and the regular file
     */
    ret = exp_metadata_write_attributes(p,fake_inode,global_attr_p,sizeof(ext_mattr_t));
    if (ret < 0)
    {
      goto error;
    }
    return 0;
  }
  /*
  ** case of a symbolic link: here we need to allocate a block for storing the link
  ** the slice associated with the link is the one associated with the fid of the file
  ** and not the fid of the parent directory
  */
  exp_trck_get_slice(global_attr_p->s.attrs.fid,&link_slice);

  fake_inode = (rozofs_inode_t*)fid;
  fake_inode_link.fid[1] = 0;
  fake_inode_link.s.key = ROZOFS_SLNK;
  fake_inode_link.s.usr_id = link_slice; 
  fake_inode_link.s.eid = trk_tb_p->eid;   

  p_link = trk_tb_p->tracking_table[ROZOFS_SLNK];
  if (p_link == NULL)
  {
    errno = ENOTSUP;
    goto error;
  }  
   /*
   ** allocate the inode
   */
   ret = exp_metadata_allocate_inode(p_link,&fake_inode_link,ROZOFS_SLNK,link_slice);
   if (ret < 0)
   { 
    goto error;
   }
   link_allocated = 1;
   /*
   ** write the link value
   */
   int len = strlen(link);
   ret = exp_metadata_write_attributes(p_link,&fake_inode_link,link,len+1);
   if (ret < 0)
   {
    goto error;
   }
   /*
   ** update the inode with the reference of the block allocated for storing the link value
   */
   global_attr_p->s.i_link_name = fake_inode_link.fid[1];
   ret = exp_metadata_write_attributes(p,fake_inode,global_attr_p,sizeof(ext_mattr_t));
   if (ret < 0)
   {  
     goto error;
   }   
   return 0;

error:
   if (inode_allocated)
   {
     exp_attr_delete(trk_tb_p,global_attr_p->s.attrs.fid);           
   }
   if (link_allocated)
   {
     memcpy(fid,&fake_inode_link,sizeof(fid_t));
     exp_attr_delete(trk_tb_p,fid);           
   }
   return -1;
}


/*
**__________________________________________________________________
*/
/**
*  Create the extended attributes of a directory/regular file or symbolic link

  create an oject according to its type. The service performs the allocation of the fid. 
  It is assumed that all the other fields of the object attributes are already been filled in.
  
  @param trk_tb_p: export attributes tracking table
  @param entry : pointer to the inode and extended attributes of the inode (header)
  @param block_ref_p : pointer to the reference of the allocated block (Not Significant if retval is -1)
  
  @retval 0 on success: (the attributes contains the lower part of the fid that is allocated by the service)
  @retval -1 on error (see errno for details)
*/
int exp_xattr_block_create(export_tracking_table_t *trk_tb_p,lv2_entry_t *entry,uint64_t *block_ref_p)
{
   rozofs_inode_t fake_inode;
   int ret;
   exp_trck_top_header_t *p = NULL;
   uint32_t slice;
   uint64_t *xattr_p;   
   /*
   ** get slice of the fid of the inode
   */
   exp_trck_get_slice(entry->attributes.s.attrs.fid,&slice); 
   /*
   ** prepare the fid for the extended block
   */
   fake_inode.fid[1] = 0;
   fake_inode.s.key = ROZOFS_EXTATTR;
   fake_inode.s.usr_id = slice; /** always the inode slice for storage */
   fake_inode.s.eid = trk_tb_p->eid;   
   
   p = trk_tb_p->tracking_table[fake_inode.s.key];
   if (p == NULL)
   {
     errno = ENOTSUP;
     return -1;    
   }   
   /*
   ** allocate the extended block
   */
   ret = exp_metadata_allocate_inode(p,&fake_inode,ROZOFS_EXTATTR,slice);
   if (ret < 0)
   { 
      return -1;
   }
   /*
   ** get the reference of the block since we will need it for the inode
   */
   *block_ref_p = fake_inode.fid[1];
   /*
   ** write back the inode reference in the header of the extended attribute block
   */
   xattr_p = entry->extended_attr_p;
   xattr_p[(ROZOFS_XATTR_BLOCK_SZ/sizeof(uint64_t))-1] = fake_inode.fid[1];
   /*
   ** write the extended attribute block  on disk
   */
   ret = exp_metadata_write_attributes(p,&fake_inode,xattr_p,ROZOFS_XATTR_BLOCK_SZ);
   if (ret < 0)
   {
     return -1;
   }
   return 0;
}

/*
**__________________________________________________________________
*/
/** store the extended attributes part of an attribute cache entry to the export's file system
 *
   @param trk_tb_p: export attributes tracking table
   @param entry: the entry used
 
   @return: 0 on success otherwise -1
 */
int export_lv2_write_xattr(export_tracking_table_t *trk_tb_p,lv2_entry_t *entry) {

   int ret;
   rozofs_inode_t fake_inode;
   exp_trck_top_header_t *p = NULL;
   
   fake_inode.fid[1] = entry->attributes.s.i_file_acl;

   p = trk_tb_p->tracking_table[ROZOFS_EXTATTR];
   if (p == NULL)
   {
     errno = ENOTSUP;
     return -1;    
   }
   if (entry->extended_attr_p == NULL)
   {
     errno = EINVAL;
     return -1;
   }
   /*
   ** read the attributes from disk
   */
   ret = exp_metadata_write_attributes(p,&fake_inode,entry->extended_attr_p,ROZOFS_XATTR_BLOCK_SZ);
   if (ret < 0)
   { 
     return -1;
   }  
   return 0; 
}

/*
**__________________________________________________________________
*/
/**
*  Create an entry in the trash for a file to delete

 
  
  @param trk_tb_p: export attributes tracking table
  @param slice: slice of the parent directory
  @param global_attr_p : pointer to the attributes relative to the object to delete
  @param link: pointer to the symbolic link (significant for ROZOFS_SLNK only)
  
  @retval 0 on success: (the attributes contains the lower part of the fid that is allocated by the service)
  @retval -1 on error (see errno for details)
*/
int exp_trash_entry_create(export_tracking_table_t *trk_tb_p,uint32_t slice,void *ptr)
{
   rozofs_inode_t fake_inode;
   int ret;
   exp_trck_top_header_t *p = NULL;
   rmfentry_disk_t *global_attr_p = (rmfentry_disk_t *)ptr;
   
   fake_inode.s.key = ROZOFS_TRASH;
   fake_inode.s.usr_id = slice; 
   fake_inode.s.eid = trk_tb_p->eid;   
   
   p = trk_tb_p->tracking_table[fake_inode.s.key];
   if (p == NULL)
   {
     errno = ENOTSUP;
     return -1;    
   }   
   /*
   ** allocate the inode
   */
   ret = exp_metadata_allocate_inode(p,&fake_inode,ROZOFS_TRASH,slice);
   if (ret < 0)
   { 
      return -1;
   }

   /*
   ** copy the reference of the trash inode
   */
   memcpy(&global_attr_p->trash_inode,&fake_inode,sizeof(rozofs_inode_t));
   /*
   ** write the metadata on disk
   */
   ret = exp_metadata_write_attributes(p,&fake_inode,global_attr_p,sizeof(rmfentry_disk_t));
  if (ret < 0)
  {
    return -1;
  }
  return 0;
}

/*
**__________________________________________________________________
*/
/**
*  Creation of one metadata tracking context (private API)

 
   @param tab_p: pointer to the global tracking context of an export
   @param index: index of the metadata tracking context  
   @param name: name of the metadata tracking context
   @param size: size of the attribute context for the metadata tracking type
   @param root_path : root path of the export
   @param create_flag : assert to 1 if tracking files MUST be created
   
   @retval 0 on success
   @retval <0 on error(see errno for details)
*/

int exp_create_one_attributes_tracking_context(export_tracking_table_t *tab_p,int index,char *name,int size,char *root_path,int create)
{
   char pathname[1024];

   sprintf(pathname,"%s/%s",root_path,name);
   if (access(pathname, F_OK) == -1) 
   {
    if (errno == ENOENT) 
    {
      /*
      ** create the directory
      */
      if (mkdir(pathname, S_IRUSR | S_IWUSR | S_IXUSR) != 0)
      goto error; 
     }
     else
     {
       severe("cannot access to %s: %s\n",pathname,strerror(errno));
       goto error;
     }
   }
   tab_p->tracking_table[index] = exp_trck_top_allocate(name,pathname,size,create);
  if (tab_p->tracking_table[index] == NULL)
  {
    goto error;
  }
  return 0;  
error:
  return -1;
}
/*
**__________________________________________________________________
*/
/**
*  Export tracking table create

   That service is called at export creation time. Its purpose is to allocate
   data structure for export attributes management.
   
   @param eid : export identifier
   @param root_path : root path of the export
   @param create_flag : assert to 1 if tracking files MUST be created
   
   @retval <> NULL: pointer to the attributes tracking table
   @retval == NULL : error (see errno for details)
*/
export_tracking_table_t *exp_create_attributes_tracking_context(uint16_t eid, char *root_path, int create)
{
   export_tracking_table_t *tab_p = NULL; 
   int ret; 
   int loop;
   
   tab_p = malloc(sizeof(export_tracking_table_t));
   if (tab_p == NULL)
   {
     /*
     ** out of memory
     */
     return NULL;
   }
   memset(tab_p,0,sizeof(export_tracking_table_t));
   tab_p->eid = eid;
   /*
   ** regular file
   */
   ret = exp_create_one_attributes_tracking_context(tab_p,ROZOFS_REG,"reg_attr",ROZOFS_TRACKING_ATTR_SIZE,root_path,create);
   if (ret < 0) goto error;
   /*
   ** create all entries
   */
   for(loop= 0; loop < EXP_TRCK_MAX_USER_ID; loop++)
   {
     ret = exp_trck_top_add_user_id(tab_p->tracking_table[ROZOFS_REG],loop);
     if (ret < 0) 
     {
       severe("reg_attr:creation failure for user_id %d \n",loop);
       goto error;   
     }  
   }
   /*
   ** directory
   */
   ret = exp_create_one_attributes_tracking_context(tab_p,ROZOFS_DIR,"dir_attr",ROZOFS_TRACKING_ATTR_SIZE,root_path,create);
   if (ret < 0) goto error;
   /*
   ** create all entries
   */
   for(loop= 0; loop < EXP_TRCK_MAX_USER_ID; loop++)
   {
     ret = exp_trck_top_add_user_id(tab_p->tracking_table[ROZOFS_DIR],loop);
     if (ret < 0) 
     {
       severe("dir_attr:creation failure for user_id %d \n",loop);
       goto error;   
     }  
   }
   /*
   ** symlink
   */
   ret = exp_create_one_attributes_tracking_context(tab_p,ROZOFS_SLNK,"dir_slnk",ROZOFS_PATH_MAX,root_path,create);
   if (ret < 0) goto error;
   for (loop= 0; loop < EXP_TRCK_MAX_USER_ID; loop++)
   {
     ret = exp_trck_top_add_user_id(tab_p->tracking_table[ROZOFS_SLNK],loop);
     if (ret < 0) 
     {
       severe("dir_slnk:creation failure for user_id %d \n",loop);
       goto error;   
     }  
   }   
   /*
   ** extended attributes
   */
   ret = exp_create_one_attributes_tracking_context(tab_p,ROZOFS_EXTATTR,"dir_xattr",ROZOFS_XATTR_BLOCK_SZ,root_path,create);
   if (ret < 0) goto error;
   for (loop= 0; loop < EXP_TRCK_MAX_USER_ID; loop++)
   {
     ret = exp_trck_top_add_user_id(tab_p->tracking_table[ROZOFS_EXTATTR],loop);
     if (ret < 0) 
     {
       severe("dir_xattr:creation failure for user_id %d \n",loop);
       goto error;   
     }  
   }   

   /*
   ** trash directory
   */
   ret = exp_create_one_attributes_tracking_context(tab_p,ROZOFS_TRASH,"dir_trash",sizeof(rmfentry_disk_t),root_path,create);
   if (ret < 0) goto error;
   for (loop= 0; loop < EXP_TRCK_MAX_USER_ID; loop++)
   {
     ret = exp_trck_top_add_user_id(tab_p->tracking_table[ROZOFS_TRASH],loop);
     if (ret < 0) 
     {
       severe("dir_xattr:creation failure for user_id %d \n",loop);
       goto error;   
     }  
   }   
   return tab_p;

error:
   exp_release_attributes_tracking_context(tab_p);
   return NULL;


}

/*
**__________________________________________________________________
*/
/**
*  Export tracking table deletion

   That service is called at export creation time. Its purpose is to allocate
   data structure for export attributes management.
   
   @param tab_p  : pointer to the attributes tracking table 
   
   @retval none
*/
void exp_release_attributes_tracking_context(export_tracking_table_t *tab_p)
{
   int i;
   
   if (tab_p == NULL) return;
   for (i = 0 ; i < ROZOFS_MAXATTR; i++)
   {
      if (tab_p->tracking_table[i] == NULL) continue;
      exp_trck_top_release(tab_p->tracking_table[i]);
   }
   free(tab_p);
}
