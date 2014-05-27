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


#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <libconfig.h>
#include <unistd.h>
#include <inttypes.h>
#include <config.h>
#include <rozofs/common/log.h>
#include <rozofs/common/xmalloc.h>
#include "export.h"
#include "econfig.h"

#define ELAYOUT     "layout"
#define EVIP        "exportd_vip"
#define EVOLUMES    "volumes"
#define EVID        "vid"
#define ECIDS       "cids"
#define ECID        "cid"
#define ESTORAGES   "storages"
#define ESIDS       "sids"
#define ESID        "sid"
#define EHOST       "host"
#define EEXPORTS    "exports"
#define EEID        "eid"
#define EROOT       "root"
#define EMD5        "md5"
#define ESQUOTA     "squota"
#define EHQUOTA     "hquota"
#define ECORES      "nbcores"
#define EGEOREP     "georep"
#define ESITE0     "site0"
#define ESITE1     "site1"
/*
** constant for exportd gateways
*/
#define EXPORTDID       "export_gateways"
#define EDAEMONID       "daemon_id"
#define EGWIDS          "gwids"
#define EGWID           "gwid"

int storage_node_config_initialize(storage_node_config_t *s, uint8_t sid,
        const char *host) {
    int status = -1;

    DEBUG_FUNCTION;

    if (sid > SID_MAX || sid < SID_MIN) {
        fatal("The SID value must be between %u and %u", SID_MIN, SID_MAX);
        goto out;
    }

    s->sid = sid;
    strncpy(s->host, host, ROZOFS_HOSTNAME_MAX);
    list_init(&s->list);

    status = 0;
out:
    return status;
}

void storage_node_config_release(storage_node_config_t *s) {
    return;
}

int cluster_config_initialize(cluster_config_t *c, cid_t cid) {
    DEBUG_FUNCTION;
    int i;

    c->cid = cid;
    for (i = 0; i <ROZOFS_GEOREP_MAX_SITE; i++) list_init(&c->storages[i]);
    list_init(&c->list);
    return 0;
}

void cluster_config_release(cluster_config_t *c) {
    list_t *p, *q;
    DEBUG_FUNCTION;
    int i;

    for (i = 0; i <ROZOFS_GEOREP_MAX_SITE; i++) 
    {
      list_for_each_forward_safe(p, q, (&c->storages[i])) {
          storage_node_config_t *entry = list_entry(p, storage_node_config_t,
                  list);
          storage_node_config_release(entry);
          list_remove(p);
          free(entry);
      }
    }
}

int volume_config_initialize(volume_config_t *v, vid_t vid, uint8_t layout,uint8_t georep) {
    DEBUG_FUNCTION;

    v->vid = vid;
    v->layout = layout;
    v->georep = georep;
    list_init(&v->clusters);
    list_init(&v->list);
    return 0;
}

void volume_config_release(volume_config_t *v) {
    list_t *p, *q;
    DEBUG_FUNCTION;

    list_for_each_forward_safe(p, q, &v->clusters) {
        cluster_config_t *entry = list_entry(p, cluster_config_t, list);
        cluster_config_release(entry);
        list_remove(p);
        free(entry);
    }
}


/**< exportd expgw */
int expgw_node_config_initialize(expgw_node_config_t *s, uint8_t gwid,
        const char *host) {
    int status = -1;

    DEBUG_FUNCTION;

    if (gwid > GWID_MAX || gwid < GWID_MIN) {
        fatal("The Exportd Gateway Id value must be between %u and %u", GWID_MIN, GWID_MAX);
        goto out;
    }

    s->gwid = gwid;
    strcpy(s->host, host);
    list_init(&s->list);

    status = 0;
out:
    return status;
}
/**< exportd expgw */
void expgw_node_config_release(expgw_node_config_t *s) {
    return;
}

/**< exportd expgw */
int expgw_config_initialize(expgw_config_t *v, int daemon_id) {
    DEBUG_FUNCTION;

    v->daemon_id = daemon_id;
    list_init(&v->expgw_node);
    list_init(&v->list);
    return 0;
}
/**< exportd expgw */
void expgw_config_release(expgw_config_t *c) {
    list_t *p, *q;
    DEBUG_FUNCTION;

    list_for_each_forward_safe(p, q, &c->expgw_node) {
        expgw_node_config_t *entry = list_entry(p, expgw_node_config_t,
                list);
        expgw_node_config_release(entry);
        list_remove(p);
        free(entry);
    }
}




int export_config_initialize(export_config_t *e, eid_t eid, vid_t vid,
        const char *root, const char *md5, uint64_t squota, uint64_t hquota) {
    DEBUG_FUNCTION;

    e->eid = eid;
    e->vid = vid;
    strncpy(e->root, root, FILENAME_MAX);
    strncpy(e->md5, md5, MD5_LEN);
    e->squota = squota;
    e->hquota = hquota;
    list_init(&e->list);
    return 0;
}

void export_config_release(export_config_t *s) {
    return;
}

int econfig_initialize(econfig_t *ec) {
    DEBUG_FUNCTION;

    list_init(&ec->volumes);
    list_init(&ec->exports);
    list_init(&ec->expgw);
    return 0;
}

void econfig_release(econfig_t *config) {
    list_t *p, *q;
    DEBUG_FUNCTION;

    list_for_each_forward_safe(p, q, &config->volumes) {
        volume_config_t *entry = list_entry(p, volume_config_t, list);
        volume_config_release(entry);
        list_remove(p);
        free(entry);
    }

    list_for_each_forward_safe(p, q, &config->exports) {
        export_config_t *entry = list_entry(p, export_config_t, list);
        export_config_release(entry);
        list_remove(p);
        free(entry);
    }
    list_for_each_forward_safe(p, q, &config->expgw) {
        expgw_config_t *entry = list_entry(p, expgw_config_t, list);
        expgw_config_release(entry);
        list_remove(p);
        free(entry);
    }
}

static int load_volumes_conf(econfig_t *ec, struct config_t *config, int elayout) {
    int status = -1, v, c, s;
    struct config_setting_t *volumes_set = NULL;

    DEBUG_FUNCTION;

    // Get settings for volumes (list of volumes)
    if ((volumes_set = config_lookup(config, EVOLUMES)) == NULL) {
        errno = ENOKEY;
        severe("can't lookup volumes setting.");
        goto out;
    }

    // For each volume
    for (v = 0; v < config_setting_length(volumes_set); v++) {

        // Check version of libconfig
#if (((LIBCONFIG_VER_MAJOR == 1) && (LIBCONFIG_VER_MINOR >= 4)) \
               || (LIBCONFIG_VER_MAJOR > 1))
        int vid,vlayout; // Volume identifier
	int vgeorep;
#else
        long int vid,vlayout; // Volume identifier
	int vgeorep;
#endif
        struct config_setting_t *vol_set = NULL; // Settings for one volume
        /* Settings of list of clusters for one volume */
        struct config_setting_t *clu_list_set = NULL;
        volume_config_t *vconfig = NULL;

        // Get settings for the volume config
        if ((vol_set = config_setting_get_elem(volumes_set, v)) == NULL) {
            errno = ENOKEY;
            severe("can't get volume setting %d.", v);
            goto out;
        }

        // Lookup vid for this volume
        if (config_setting_lookup_int(vol_set, EVID, &vid) == CONFIG_FALSE) {
            errno = ENOKEY;
            severe("can't lookup vid setting for volume idx: %d.", v);
            goto out;
        }

        // Check whether a layout is specified for this volume in the
        // configuration file or take the layout from the export default value
        if (config_setting_lookup_int(vol_set, ELAYOUT,
                &vlayout) == CONFIG_FALSE) {
            // No specific layout given for this volume.
            // Get the export default layout.
            vlayout = elayout;
        }

        // Check whether geo-replication is specified for this volume in the
        // configuration file, default value is none
        if (config_setting_lookup_int(vol_set, EGEOREP,
                &vgeorep) == CONFIG_FALSE) {
            // No specific layout given for this volume.
            // Get the export default geo-replication.
            vgeorep = 0;
        }
	if (vgeorep!= 0)
	{
	  /*
	  ** the site file MUST exist
	  */
	  if (rozofs_no_site_file)
	  {
	    severe("RozoFS site file is missing (%s/rozofs_site)",ROZOFS_CONFIG_DIR);
	    goto out;
	  }
	}
        // Allocate new volume_config
        vconfig = (volume_config_t *) xmalloc(sizeof (volume_config_t));
        if (volume_config_initialize(vconfig, (vid_t) vid, (uint8_t) vlayout,(uint8_t)vgeorep) != 0) {
            severe("can't initialize volume.");
            goto out;
        }

        // Get settings for clusters for this volume
        if ((clu_list_set = config_setting_get_member(vol_set, ECIDS)) == NULL) {
            errno = ENOKEY;
            severe("can't get cids setting for volume idx: %d.", v);
            goto out;
        }

        // For each cluster of this volume
        for (c = 0; c < config_setting_length(clu_list_set); c++) {

            // Check version of libconfig
#if (((LIBCONFIG_VER_MAJOR == 1) && (LIBCONFIG_VER_MINOR >= 4)) \
               || (LIBCONFIG_VER_MAJOR > 1))
            int cid;
#else
            long int cid;
#endif
            struct config_setting_t *stor_set;
            struct config_setting_t *clu_set;
            cluster_config_t *cconfig;

            // Get settings for this cluster
            if ((clu_set = config_setting_get_elem(clu_list_set, c)) == NULL) {
                errno = ENOKEY;
                severe("can't get cluster setting for volume idx: %d cluster idx: %d.", v, c);
                goto out;
            }

            // Lookup cid for this cluster
            if (config_setting_lookup_int(clu_set, ECID, &cid) == CONFIG_FALSE) {
                errno = ENOKEY;
                severe("can't lookup cid for volume idx: %d cluster idx: %d.", v, c);
                goto out;
            }

            // Allocate a new cluster_config
            cconfig = (cluster_config_t *) xmalloc(sizeof (cluster_config_t));
            if (cluster_config_initialize(cconfig, (cid_t) cid) != 0) {
                severe("can't initialize cluster config.");
            }

            // Get settings for sids for this cluster
            if ((stor_set = config_setting_get_member(clu_set, ESIDS)) == NULL) {
                errno = ENOKEY;
                severe("can't get sids for volume idx: %d cluster idx: %d.", v, c);
                goto out;
            }

            for (s = 0; s < config_setting_length(stor_set); s++) {

                struct config_setting_t *mstor_set = NULL;
                // Check version of libconfig
#if (((LIBCONFIG_VER_MAJOR == 1) && (LIBCONFIG_VER_MINOR >= 4)) \
               || (LIBCONFIG_VER_MAJOR > 1))
                int sid;
#else
                long int sid;
#endif
                const char *host;
                storage_node_config_t *snconfig = NULL;

                // Get settings for the storage_config
                if ((mstor_set = config_setting_get_elem(stor_set, s)) == NULL) {
                    errno = ENOKEY;
                    severe("can't get storage config for volume idx: %d\
                             , cluster idx: %d, storage idx: %d.", v, c, s);
                    goto out;
                }

                // Lookup sid for this storage
                if (config_setting_lookup_int(mstor_set, ESID, &sid) == CONFIG_FALSE) {
                    errno = ENOKEY;
                    severe("can't get sid for volume idx: %d\
                             , cluster idx: %d,  storage idx: %d.", v, c, s);
                    goto out;
                }
                if (vgeorep == 0)
		{
                  // Lookup hostname for this storage
                  if (config_setting_lookup_string(mstor_set, EHOST, &host) == CONFIG_FALSE) {
                      errno = ENOKEY;
                      severe("can't get host for volume idx: %d\
                               , cluster idx: %d,  storage idx: %d.", v, c, s);
                      goto out;
                  }
                  // Check length of storage hostname
                  if (strlen(host) > ROZOFS_HOSTNAME_MAX) {
                      errno = ENAMETOOLONG;
                      severe("Storage hostname length (volume idx: %d\
                               , cluster idx: %d, storage idx: %d) must be lower\
                           than %d.", v, c, s, ROZOFS_HOSTNAME_MAX);
                      goto out;
                  }

                  // Allocate a new storage_config
                  snconfig = (storage_node_config_t *) xmalloc(sizeof (storage_node_config_t));
                  if (storage_node_config_initialize(snconfig, (uint8_t) sid, host) != 0) {
                      severe("can't initialize storage node config.");

                  }

                  // Add it to the cluster.
                  list_push_back((&cconfig->storages[0]), &snconfig->list);
		}
		else
		{

                  // Lookup hostname for this storage
                  if (config_setting_lookup_string(mstor_set, ESITE0, &host) == CONFIG_FALSE) {
                      errno = ENOKEY;
                      severe("can't get host for volume idx: %d\
                               , cluster idx: %d,  storage idx: %d.", v, c, s);
                      goto out;
                  }
                  // Check length of storage hostname
                  if (strlen(host) > ROZOFS_HOSTNAME_MAX) {
                      errno = ENAMETOOLONG;
                      severe("Storage hostname length (volume idx: %d\
                               , cluster idx: %d, storage idx: %d) must be lower\
                           than %d.", v, c, s, ROZOFS_HOSTNAME_MAX);
                      goto out;
                  }

                  // Allocate a new storage_config
                  snconfig = (storage_node_config_t *) xmalloc(sizeof (storage_node_config_t));
                  if (storage_node_config_initialize(snconfig, (uint8_t) sid, host) != 0) {
                      severe("can't initialize storage node config.");

                  }

                  // Add it to the cluster.
                  list_push_back((&cconfig->storages[0]), &snconfig->list);
                  // Lookup hostname for this storage
                  if (config_setting_lookup_string(mstor_set, ESITE1, &host) == CONFIG_FALSE) {
                      errno = ENOKEY;
                      severe("can't get host for volume idx: %d\
                               , cluster idx: %d,  storage idx: %d.", v, c, s);
                      goto out;
                  }
                  // Check length of storage hostname
                  if (strlen(host) > ROZOFS_HOSTNAME_MAX) {
                      errno = ENAMETOOLONG;
                      severe("Storage hostname length (volume idx: %d\
                               , cluster idx: %d, storage idx: %d) must be lower\
                           than %d.", v, c, s, ROZOFS_HOSTNAME_MAX);
                      goto out;
                  }

                  // Allocate a new storage_config
                  snconfig = (storage_node_config_t *) xmalloc(sizeof (storage_node_config_t));
                  if (storage_node_config_initialize(snconfig, (uint8_t) sid, host) != 0) {
                      severe("can't initialize storage node config.");

                  }

                  // Add it to the cluster.
                  list_push_back((&cconfig->storages[1]), &snconfig->list);
		}
				

            }

            // Add the cluster to the volume
            list_push_back(&vconfig->clusters, &cconfig->list);

        } // End add cluster

        // Add this volume to the list of volumes
        list_push_back(&ec->volumes, &vconfig->list);
    } // End add volume

    status = 0;
out:
    return status;
}

static int load_expgw_conf(econfig_t *ec, struct config_t *config) {
    int status = -1, v, s;
    struct config_setting_t *master_expgw_set = NULL;

    DEBUG_FUNCTION;

    // Get settings for expgw (list of expgw)
    if ((master_expgw_set = config_lookup(config, EXPORTDID)) == NULL) {
//        errno = ENOKEY;
//        severe("can't lookup expgw setting.");
        status = 0;
        goto out;
    }

    // For each expgw
    for (v = 0; v < config_setting_length(master_expgw_set); v++) {

        // Check version of libconfig
#if (((LIBCONFIG_VER_MAJOR == 1) && (LIBCONFIG_VER_MINOR >= 4)) \
               || (LIBCONFIG_VER_MAJOR > 1))
        int daemon_id; // Replica identifier
#else
        long int daemon_id; // Replica identifier
#endif
        struct config_setting_t *expgw_set = NULL; // Settings for one volume
        struct config_setting_t *mexpgw_set = NULL; // Settings for one volume
        /* Settings of list of clusters for one volume */
        expgw_config_t *rconfig = NULL;

        // Get settings for the volume config
        if ((expgw_set = config_setting_get_elem(master_expgw_set, v)) == NULL) {
            errno = ENOKEY;
            severe("can't get expgw setting %d.", v);
            goto out;
        }

        // Lookup daemon_id for this volume
        if (config_setting_lookup_int(expgw_set, EDAEMONID, &daemon_id) == CONFIG_FALSE) {
            errno = ENOKEY;
            severe("can't lookup daemon_id setting for volume idx: %d.", v);
            goto out;
        }

        // Allocate new expgw_config
        rconfig = (expgw_config_t *) xmalloc(sizeof (expgw_config_t));
        if (expgw_config_initialize(rconfig, (int) daemon_id) != 0) {
            severe("can't initialize expgw.");
            goto out;
        }

            // Get settings for gwids for this exportd expgw
            if ((mexpgw_set = config_setting_get_member(expgw_set, EGWIDS)) == NULL) {
                errno = ENOKEY;
                severe("can't get gwids for daemon_id idx: %d ", v);
                goto out;
            }

            for (s = 0; s < config_setting_length(mexpgw_set); s++) {

                struct config_setting_t *node_expgw_set = NULL;
                // Check version of libconfig
#if (((LIBCONFIG_VER_MAJOR == 1) && (LIBCONFIG_VER_MINOR >= 4)) \
               || (LIBCONFIG_VER_MAJOR > 1))
                int gwid;
#else
                long int gwid;
#endif
                const char *host;
                expgw_node_config_t *snconfig = NULL;

                // Get settings for the expgw_node_config_t
                if ((node_expgw_set = config_setting_get_elem(mexpgw_set, s)) == NULL) {
                    errno = ENOKEY;
                    severe("can't get expgw node config for export expgw: %d\
                             , expgw node idx: %d.", v, s);
                    goto out;
                }

                // Lookup sid for this storage
                if (config_setting_lookup_int(node_expgw_set, EGWID, &gwid) == CONFIG_FALSE) {
                    errno = ENOKEY;
                    severe("can't get gwid for export expgw: %d\
                               , expgw node idx: %d.", v, s);
                    goto out;
                }

                // Lookup hostname for this storage
                if (config_setting_lookup_string(node_expgw_set, EHOST, &host) == CONFIG_FALSE) {
                    errno = ENOKEY;
                    severe("can't get host for export expgw: %d\
                               , expgw node idx: %d.", v, s);
                    goto out;
                }

                // Allocate a new expgw_node_config
                snconfig = (expgw_node_config_t *) xmalloc(sizeof (expgw_node_config_t));
                if (expgw_node_config_initialize(snconfig, (uint8_t) gwid, host) != 0) {
                    severe("can't initialize storage node config.");
                }

            // Add the expgw node to the exportd expgw
            list_push_back(&rconfig->expgw_node, &snconfig->list);

        } // End add cluster

        // Add this volume to the list of expgw
        list_push_back(&ec->expgw, &rconfig->list);
    } // End add volume

    status = 0;
out:
    return status;
}



static int strquota_to_nbblocks(const char *str, uint64_t *blocks) {
    int status = -1;
    char *unit;
    uint64_t value;

    errno = 0;
    value = strtol(str, &unit, 10);
    if ((errno == ERANGE && (value == LONG_MAX || value == LONG_MIN))
            || (errno != 0 && value == 0)) {
        goto out;
    }

    // no digit, no quota
    if (unit == str) {
        *blocks = 0;
        status = 0;
        goto out;
    }

    switch (*unit) {
        case 'K':
            *blocks = 1024 * value / ROZOFS_BSIZE;
            break;
        case 'M':
            *blocks = 1024 * 1024 * value / ROZOFS_BSIZE;
            break;
        case 'G':
            *blocks = 1024 * 1024 * 1024 * value / ROZOFS_BSIZE;
            break;
        case 'T':
            // 0x10000000000 = 1024*1024*1024*1024
            *blocks = 0x10000000000 * value / ROZOFS_BSIZE;
            break;
        default: // no unit -> nb blocks
            *blocks = value;
            break;
    }

    status = 0;

out:
    return status;
}

static int load_exports_conf(econfig_t *ec, struct config_t *config) {
    int status = -1, i;
    struct config_setting_t *export_set = NULL;

    DEBUG_FUNCTION;

    // Get the exports settings
    if ((export_set = config_lookup(config, EEXPORTS)) == NULL) {
        errno = ENOKEY;
        severe("can't lookup exports setting.");
        goto out;
    }

    // For each export
    for (i = 0; i < config_setting_length(export_set); i++) {
        struct config_setting_t *mfs_setting = NULL;
        const char *root;
        const char *md5;
        // Check version of libconfig
#if (((LIBCONFIG_VER_MAJOR == 1) && (LIBCONFIG_VER_MINOR >= 4)) \
               || (LIBCONFIG_VER_MAJOR > 1))
        int eid; // Export identifier
        int vid; // Volume identifier
#else
        long int eid; // Export identifier
        long int vid; // Volume identifier
#endif
        const char *str;
        uint64_t squota;
        uint64_t hquota;
        export_config_t *econfig = NULL;

        if ((mfs_setting = config_setting_get_elem(export_set, i)) == NULL) {
            errno = ENOKEY;
            severe("can't get export idx: %d.", i);
            goto out;
        }

        if (config_setting_lookup_int(mfs_setting, EEID, &eid) == CONFIG_FALSE) {
            errno = ENOKEY;
            severe("can't look up eid for export idx: %d", i);
            goto out;
        }

        if (config_setting_lookup_string(mfs_setting, EROOT, &root) == CONFIG_FALSE) {
            errno = ENOKEY;
            severe("can't look up root path for export idx: %d", i);
            goto out;
        }

        // Check root path length
        if (strlen(root) > FILENAME_MAX) {
            errno = ENAMETOOLONG;
            severe("root path length for export idx: %d must be lower than %d.",
                    i, FILENAME_MAX);
            goto out;
        }

        if (config_setting_lookup_string(mfs_setting, EMD5, &md5) == CONFIG_FALSE) {
            errno = ENOKEY;
            severe("can't look up md5 for export idx: %d", i);
            goto out;
        }

        // Check md5 length
        if (strlen(md5) > MD5_LEN) {
            errno = EINVAL;
            severe("md5 crypt length for export idx: %d must be lower than %d.",
                    i, MD5_LEN);
            goto out;
        }

        if (config_setting_lookup_string(mfs_setting, ESQUOTA, &str) == CONFIG_FALSE) {
            errno = ENOKEY;
            severe("can't look up squota for export idx: %d", i);
            goto out;
        }

        if (strquota_to_nbblocks(str, &squota) != 0) {
            severe("%s: can't convert to quota)", str);
            goto out;
        }

        if (config_setting_lookup_string(mfs_setting, EHQUOTA, &str) == CONFIG_FALSE) {
            errno = ENOKEY;
            severe("can't look up hquota for export idx: %d", i);
            goto out;
        }

        if (strquota_to_nbblocks(str, &hquota) != 0) {
            severe("%s: can't convert to quota)", str);
            goto out;
        }

        // Lookup volume identifier
        if (config_setting_lookup_int(mfs_setting, EVID, &vid) == CONFIG_FALSE) {
            errno = ENOKEY;
            severe("can't look up vid for export idx: %d", i);
            goto out;
        }

        econfig = xmalloc(sizeof (export_config_t));
        if (export_config_initialize(econfig, (eid_t) eid, (vid_t) vid, root,
                md5, squota, hquota) != 0) {
            severe("can't initialize export config.");
        }
        // Initialize export

        // Add this export to the list of exports
        list_push_back(&ec->exports, &econfig->list);

    }
    status = 0;
out:
    return status;
}

int econfig_read(econfig_t *config, const char *fname) {
    int status = -1;
    //const char *host;
    config_t cfg;
    // Check version of libconfig
#if (((LIBCONFIG_VER_MAJOR == 1) && (LIBCONFIG_VER_MINOR >= 4)) \
               || (LIBCONFIG_VER_MAJOR > 1))
    int layout, nbCores;
#else

    long int layout, nbCores;
#endif

    DEBUG_FUNCTION;

    config_init(&cfg);

    if (config_read_file(&cfg, fname) == CONFIG_FALSE) {
        errno = EIO;
        severe("can't read %s : %s.", fname, config_error_text(&cfg));
        goto out;
    }
    
    if (!config_lookup_int(&cfg, ECORES, &nbCores)) {
       config->nb_cores = 2;
    }
    else {
       config->nb_cores = nbCores;
    }
    
    if (!config_lookup_int(&cfg, ELAYOUT, &layout)) {
        errno = ENOKEY;
        severe("can't lookup layout setting.");
        goto out;
    }
    config->layout = (uint8_t) layout;

#if 0 // not needed since exportgateway code is inactive
    if (!config_lookup_string(&cfg, EVIP, &host)) {
        errno = ENOKEY;
        severe("can't lookup exportd vip setting.");
        goto out;
    }

    // Check length of export hostname
    if (strlen(host) > ROZOFS_HOSTNAME_MAX) {
        errno = ENAMETOOLONG;
        severe(" hostname length  must be lower\
             than %d.", ROZOFS_HOSTNAME_MAX);
        goto out;
    }
    strncpy(config->exportd_vip, host, ROZOFS_HOSTNAME_MAX);
#endif
    
    if (load_volumes_conf(config, &cfg, layout) != 0) {
        severe("can't load volume config.");
        goto out;
    }
    /**< exportd replca -> load the configuration */
    if (load_expgw_conf(config, &cfg) != 0) {
        severe("can't load export expgw config.");
        goto out;
    }

    if (load_exports_conf(config, &cfg) != 0) {
        severe("can't load exports config.");
        goto out;
    }

    status = 0;
out:
    config_destroy(&cfg);
    return status;
}

static int econfig_validate_storages(cluster_config_t *config) {
    int status = -1;
    list_t *p, *q;
    DEBUG_FUNCTION;
    int i;
    for (i = 0; i <ROZOFS_GEOREP_MAX_SITE; i++) {
      list_for_each_forward(p, (&config->storages[i])) {
          storage_node_config_t *e1 = list_entry(p, storage_node_config_t, list);

          list_for_each_forward(q, (&config->storages[i])) {
              storage_node_config_t *e2 = list_entry(q, storage_node_config_t, list);
              if (e1 == e2)
                  continue;
              if (e1->sid == e2->sid) {
                  severe("duplicated sid: %d", e1->sid);
                  errno = EINVAL;
                  goto out;
              }
          }
      }
    }

    status = 0;
out:
    return status;
}

/**< expgw exportd */
static int econfig_validate_expgw_node(expgw_config_t *config) {
    int status = -1;
    list_t *p, *q;
    DEBUG_FUNCTION;

    list_for_each_forward(p, &config->expgw_node) {
        expgw_node_config_t *e1 = list_entry(p, expgw_node_config_t, list);

        list_for_each_forward(q, &config->expgw_node) {
            expgw_node_config_t *e2 = list_entry(q, expgw_node_config_t, list);
            if (e1 == e2)
                continue;
            if (e1->gwid == e2->gwid) {
                severe("duplicated gwid: %d", e1->gwid);
                errno = EINVAL;
                goto out;
            }
        }
    }

    status = 0;
out:
    return status;
}
static int econfig_validate_clusters(volume_config_t *config) {
    int status = -1;
    list_t *p, *q;
    DEBUG_FUNCTION;

    list_for_each_forward(p, &config->clusters) {
        cluster_config_t *e1 = list_entry(p, cluster_config_t, list);

        list_for_each_forward(q, &config->clusters) {
            cluster_config_t *e2 = list_entry(q, cluster_config_t, list);
            if (e1 == e2)
                continue;
            if (e1->cid == e2->cid) {
                severe("duplicated cid: %d", e1->cid);
                errno = EINVAL;
                goto out;
            }
        }
        if (econfig_validate_storages(e1) != 0) {
            severe("invalid storage.");
            goto out;
        }
    }

    status = 0;
out:
    return status;
}

/** Checks if maximum storage limits is exceeded for a given volume
 *
 * @param config: volume configuration
 *
 * @return: 0 on success -1 otherwise (errno is set)
 */
static int econfig_validate_storage_nb(volume_config_t *config) {
    int status = -1;
    list_t *q, *r;
    int curr_stor_idx = 0;
    int j = 0;
    int i = 0;
    int exist = 0;

    DEBUG_FUNCTION;

    // Internal structure for store storage node

    typedef struct stor_node_check {
        char host[ROZOFS_HOSTNAME_MAX];
        uint8_t sids_nb;
    } stor_node_check_t;

    stor_node_check_t stor_nodes[STORAGE_NODES_MAX];

    memset(stor_nodes, 0, STORAGE_NODES_MAX * sizeof (stor_node_check_t));

    for (j = 0; j <ROZOFS_GEOREP_MAX_SITE; j++) 
    {

     // For each cluster

     list_for_each_forward(q, &config->clusters) {
         cluster_config_t *c = list_entry(q, cluster_config_t, list);

         // For each storage

         list_for_each_forward(r, (&c->storages[j])) {
             storage_node_config_t *s = list_entry(r, storage_node_config_t,
                     list);

             exist = 0;

             // Check if the storage hostname already exists
             for (i = 0; i < curr_stor_idx; i++) {

                 if (strcmp(s->host, stor_nodes[i].host) == 0) {
                     // This physical storage node exist
                     stor_nodes[i].sids_nb++;
                     // Check nb. of storages with this hostname
                     if (stor_nodes[i].sids_nb > STORAGES_MAX_BY_STORAGE_NODE) {
                         severe("Too many storages with the hostname=%s"
                                 " in volume with vid=%u (number max is %d)",
                                 s->host, config->vid,
                                 STORAGES_MAX_BY_STORAGE_NODE);
                         errno = EINVAL;
                         goto out;
                     }
                     exist = 1;
                     break;
                 }
             }

             // This physical storage node doesn't exist
             if (exist == 0) {

                 if ((curr_stor_idx + 1) > STORAGE_NODES_MAX) {
                     severe("Too many storages in volume with vid=%u"
                             " (number max is %d)",
                             config->vid, STORAGE_NODES_MAX);
                     errno = EINVAL;
                     goto out;
                 }
                 // Add this storage node
                 strncpy(stor_nodes[curr_stor_idx].host, s->host,
                         ROZOFS_HOSTNAME_MAX);
                 stor_nodes[curr_stor_idx].sids_nb++;
                 // Increments the nb. of physical storage nodes
                 curr_stor_idx++;
             }
         }
     }
    }
    status = 0;
out:
    return status;
}
/*< expgw exportd  */
static int econfig_validate_expgw(econfig_t *config) {
    int status = -1;
    list_t *p, *q;
    DEBUG_FUNCTION;

    list_for_each_forward(p, &config->expgw) {
        expgw_config_t *e1 = list_entry(p, expgw_config_t, list);

        list_for_each_forward(q, &config->expgw) {
            expgw_config_t *e2 = list_entry(q, expgw_config_t, list);
            if (e1 == e2)
                continue;
            if (e1->daemon_id == e2->daemon_id) {
                severe("duplicated daemon_id: %d", e1->daemon_id);
                errno = EINVAL;
                goto out;
            }
        }
        if (econfig_validate_expgw_node(e1) != 0) {
            severe("invalid expgw node.");
            goto out;
        }
    }

    status = 0;
out:
    return status;
}

static int econfig_validate_volumes(econfig_t *config) {
    int status = -1;
    list_t *p, *q;
    DEBUG_FUNCTION;

    list_for_each_forward(p, &config->volumes) {
        volume_config_t *e1 = list_entry(p, volume_config_t, list);


	if (e1->layout < LAYOUT_2_3_4 || e1->layout > LAYOUT_8_12_16) {
            severe("unknown layout: %d.", e1->layout);
            errno = EINVAL;
            goto out;
	}

        list_for_each_forward(q, &config->volumes) {
            volume_config_t *e2 = list_entry(q, volume_config_t, list);
            if (e1 == e2)
                continue;
            if (e1->vid == e2->vid) {
                severe("duplicated vid: %d", e1->vid);
                errno = EINVAL;
                goto out;
            }
        }

        if (econfig_validate_storage_nb(e1)) {
            goto out;
        }

        if (econfig_validate_clusters(e1) != 0) {
            severe("invalid cluster.");
            goto out;
        }
    }

    status = 0;
out:
    return status;
}

static int econfig_validate_exports(econfig_t *config) {
    int status = -1;
    list_t *p, *q, *r;
    int found = 0;
    DEBUG_FUNCTION;

    list_for_each_forward(p, &config->exports) {
        export_config_t *e1 = list_entry(p, export_config_t, list);

        list_for_each_forward(q, &config->exports) {
            export_config_t *e2 = list_entry(q, export_config_t, list);
            if (e1 == e2)
                continue;
            if (e1->eid == e2->eid) {
                severe("duplicated eid: %d", e1->eid);
                errno = EINVAL;
                goto out;
            }
            if (strcmp(e1->root, e2->root) == 0) {
                severe("duplicated root: %s", e1->root);
                errno = EINVAL;
                goto out;
            }
        }
        found = 0;

        list_for_each_forward(r, &config->volumes) {
            volume_config_t *e3 = list_entry(r, volume_config_t, list);
            if (e1->vid == e3->vid) {
                found = 1;
                break;
            }
        }
        if (found != 1) {
            severe("invalid vid for eid: %d", e1->eid);
            errno = EINVAL;
            goto out;
        }
        if (access(e1->root, F_OK) != 0) {
            severe("can't access %s: %s.", e1->root, strerror(errno));
            goto out;
        }
    }

    status = 0;
out:
    return status;
}

int econfig_validate(econfig_t *config) {
    int status = -1;
    DEBUG_FUNCTION;

    if (config->layout < LAYOUT_2_3_4 || config->layout > LAYOUT_8_12_16) {
        severe("unknown layout: %d.", config->layout);
        errno = EINVAL;
        goto out;
    }

    if (econfig_validate_volumes(config) != 0) {
        severe("invalid volume.");
        goto out;
    }

    if (econfig_validate_expgw(config) != 0) {
        severe("invalid expgw.");
        goto out;
    }
    if (econfig_validate_exports(config) != 0) {
        severe("invalid export.");
        goto out;
    }

    status = 0;
out:
    return status;
}

// check whenever we can load to coming from from without breaking
// exportd consistency

int econfig_check_consistency(econfig_t *from, econfig_t *to) {
    DEBUG_FUNCTION;

    if (from->layout != to->layout) {
        severe("inconsistent layout %d vs %d.", from->layout, to->layout);
        return -1;
    }

    //TODO
    return 0;
}

int econfig_print(econfig_t *config) {
    list_t *p;
    int i;
    printf("layout: %d\n", config->layout);
    printf("volume: \n");

    list_for_each_forward(p, &config->volumes) {
        list_t *q;
        volume_config_t *vconfig = list_entry(p, volume_config_t, list);
        printf("vid: %d\n", vconfig->vid);
        printf("layout: %d\n", vconfig->layout);

        list_for_each_forward(q, &vconfig->clusters) {
            list_t *r;
            cluster_config_t *cconfig = list_entry(q, cluster_config_t, list);
            printf("cid: %d\n", cconfig->cid);
            for (i = 0; i <ROZOFS_GEOREP_MAX_SITE; i++) {
              list_for_each_forward(r, (&cconfig->storages[i])) {
                  storage_node_config_t *sconfig = list_entry(r, storage_node_config_t, list);
                  printf("sid: %d\n", sconfig->sid);
                  printf("host: %s\n", sconfig->host);
              }
	    }
        }
    }

    list_for_each_forward(p, &config->exports) {
        export_config_t *e = list_entry(p, export_config_t, list);
        printf("eid: %d, vid:%d,  root: %s, squota:%"PRIu64", hquota: %"PRIu64"\n", e->eid, e->vid, e->root, e->squota, e->hquota);
    }
    return 0;
}
