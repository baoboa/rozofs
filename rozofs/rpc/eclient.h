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


#ifndef _ECLIENT_H
#define _ECLIENT_H

#include <rozofs/rozofs.h>
#include <rozofs/common/dist.h>
#include <rozofs/common/mattr.h>
#include <rozofs/rozofs_srv.h>

#include "rpcclt.h"
#include "sclient.h"

typedef struct mstorage {
    char host[ROZOFS_HOSTNAME_MAX];
    sclient_t sclients[STORAGE_NODE_PORTS_MAX];
    sid_t sids[STORAGES_MAX_BY_STORAGE_NODE];
    cid_t cids[STORAGES_MAX_BY_STORAGE_NODE];
    uint8_t sclients_nb;
    int     lbg_id;           /**< load balancing group reference */
    int     thread_started;   /**< asserted to 1 when the connect_storage is started */
    sid_t sids_nb;
    list_t list;
} mstorage_t;

typedef struct exportclt {
    char host[ROZOFS_HOSTNAME_MAX];
    char *root;
    char *passwd;
    eid_t eid;
    list_t storages; // XXX: Need a lock?
    uint8_t layout; // Layout for this export
    fid_t rfid;
    uint32_t bufsize;
    uint32_t min_read_size;
    uint32_t retries;
    rpcclt_t rpcclt;
    struct timeval timeout;
} exportclt_t;

extern uint32_t exportd_configuration_file_hash; /**< hash value of the configuration file */


int exportclt_initialize(exportclt_t * clt, const char *host, char *root,int site_number,
        const char *passwd, uint32_t bufsize, uint32_t min_read_size, uint32_t retries,
        struct timeval timeout);

int exportclt_reload(exportclt_t * clt);

void exportclt_release(exportclt_t * clt);

int exportclt_stat(exportclt_t * clt, estat_t * st);

int exportclt_lookup(exportclt_t * clt, fid_t parent, char *name,
        mattr_t * attrs);

int exportclt_getattr(exportclt_t * clt, fid_t fid, mattr_t * attrs);

int exportclt_setattr(exportclt_t * clt, fid_t fid, mattr_t * attrs, int to_set);

int exportclt_readlink(exportclt_t * clt, fid_t fid, char *link);

int exportclt_link(exportclt_t * clt, fid_t inode, fid_t newparent, char *newname, mattr_t * attrs);

int exportclt_mknod(exportclt_t * clt, fid_t parent, char *name, uint32_t uid,
        uint32_t gid, mode_t mode, mattr_t * attrs);

int exportclt_mkdir(exportclt_t * clt, fid_t parent, char *name, uint32_t uid,
        uint32_t gid, mode_t mode, mattr_t * attrs);

int exportclt_unlink(exportclt_t * clt, fid_t pfid, char *name, fid_t * fid);

int exportclt_rmdir(exportclt_t * clt, fid_t pfid, char *name, fid_t * fid);

int exportclt_symlink(exportclt_t * clt, char *link, fid_t parent, char *name,
        mattr_t * attrs);

int exportclt_rename(exportclt_t * clt, fid_t parent, char *name, fid_t newparent, char *newname, fid_t * fid);

//int64_t exportclt_read(exportclt_t * clt, fid_t fid, uint64_t off,
//        uint32_t len);
//
//int exportclt_read_block(exportclt_t * clt, fid_t fid, bid_t bid, uint32_t n,
//        dist_t * d);

//int64_t exportclt_read_block(exportclt_t * clt, fid_t fid, uint64_t off, uint32_t len, dist_t * d);

dist_t * exportclt_read_block(exportclt_t * clt, fid_t fid, uint64_t off, uint32_t len, int64_t * length);

//int64_t exportclt_write(exportclt_t * clt, fid_t fid, uint64_t off,
//        uint32_t len);

int64_t exportclt_write_block(exportclt_t * clt, fid_t fid, bid_t bid, uint32_t n, dist_t d, uint64_t off, uint32_t len);

int exportclt_readdir(exportclt_t * clt, fid_t fid, uint64_t * cookie, child_t ** children, uint8_t * eof);

int exportclt_setxattr(exportclt_t * clt, fid_t fid, char * name, void * value,
        uint64_t size, uint8_t flags);

int exportclt_getxattr(exportclt_t * clt, fid_t fid, char * name, void * value,
        uint64_t size, uint64_t * size2);

int exportclt_removexattr(exportclt_t * clt, fid_t fid, char * name);

int exportclt_listxattr(exportclt_t * clt, fid_t fid, char * list,
        uint64_t size, uint64_t * size2);

/* not used anymore
int exportclt_open(exportclt_t * clt, fid_t fid);

int exportclt_close(exportclt_t * clt, fid_t fid);
 */

#endif
