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

#ifndef _ECONFIG_H
#define _ECONFIG_H

#include <stdio.h>
#include <limits.h>

#include <rozofs/rozofs.h>
#include <rozofs/common/list.h>

#define MD5_LEN  22

typedef struct storage_node_config {
    sid_t sid;
    char host[ROZOFS_HOSTNAME_MAX];
    list_t list;
} storage_node_config_t;

typedef struct cluster_config {
    cid_t cid;
    list_t storages[ROZOFS_GEOREP_MAX_SITE];
    list_t list;
} cluster_config_t;

typedef struct volume_config {
    vid_t vid;
    uint8_t layout;    
    uint8_t georep;    
    list_t clusters;
    list_t list;
} volume_config_t;

typedef struct export_config {
    eid_t eid;
    vid_t vid;
    char root[FILENAME_MAX];
    char md5[MD5_LEN];
    uint64_t squota;
    uint64_t hquota;
    list_t list;
} export_config_t;



/**< exportd expgw */

typedef struct expgw_node_config {
    int gwid;
    char host[ROZOFS_HOSTNAME_MAX];
    list_t list;
} expgw_node_config_t;


typedef struct expgw_config {
    int daemon_id;
    list_t expgw_node;
    list_t list;
} expgw_config_t;


typedef struct econfig {
    int    nb_cores;
    uint8_t layout; ///< layout used for this exportd
    char   exportd_vip[ROZOFS_HOSTNAME_MAX]; ///< virtual IP address of the exportd
    list_t volumes;
    list_t exports;
    list_t expgw;   /*< exportd gateways */

} econfig_t;

int econfig_initialize(econfig_t *config);

void econfig_release(econfig_t *config);

int econfig_read(econfig_t *config, const char *fname);

int econfig_validate(econfig_t *config);

int econfig_check_consistency(econfig_t *from, econfig_t *to);

int econfig_print(econfig_t *config);

#endif
