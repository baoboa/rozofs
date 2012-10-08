/*
  Copyright (c) 2010 Fizians SAS. <http://www.fizians.com>
  This file is part of Rozofs.

  Rozofs is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published
  by the Free Software Foundation; either version 3 of the License,
  or (at your option) any later version.

  Rozofs is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see
  <http://www.gnu.org/licenses/>.
 */

#include <limits.h>
#include <errno.h>

#include "log.h"
#include "storage.h"
#include "storaged.h"
#include "mproto.h"
#include "xmalloc.h"

void *mp_null_1_svc(void *args, struct svc_req *req) {
    DEBUG_FUNCTION;
    return 0;
}

mp_status_ret_t *mp_remove_1_svc(mp_remove_arg_t * args, struct svc_req * req) {
    static mp_status_ret_t ret;
    storage_t *st = 0;
    DEBUG_FUNCTION;

    ret.status = MP_FAILURE;
    if ((st = storaged_lookup(args->sid)) == 0) {
        ret.mp_status_ret_t_u.error = errno;
        goto out;
    }
    if (storage_rm_file(st, args->fid) != 0 && errno != ENOENT) {
        ret.mp_status_ret_t_u.error = errno;
        goto out;
    }

    ret.status = MP_SUCCESS;
out:
    return &ret;
}

mp_stat_ret_t *mp_stat_1_svc(uint16_t * sid, struct svc_req * req) {
    static mp_stat_ret_t ret;
    storage_t *st = 0;
    sstat_t sstat;
    DEBUG_FUNCTION;

    ret.status = MP_FAILURE;
    if ((st = storaged_lookup(*sid)) == 0) {
        ret.mp_stat_ret_t_u.error = errno;
        goto out;
    }
    if (storage_stat(st, &sstat) != 0) {
        ret.mp_stat_ret_t_u.error = errno;
        goto out;
    }
    ret.mp_stat_ret_t_u.sstat.size = sstat.size;
    ret.mp_stat_ret_t_u.sstat.free = sstat.free;
    ret.status = MP_SUCCESS;
out:
    return &ret;
}

mp_ports_ret_t *mp_ports_1_svc(void *args, struct svc_req * req) {
    static mp_ports_ret_t ret;
/*
    int i = 0;
*/

    DEBUG_FUNCTION;

    ret.status = MP_FAILURE;

    memset(&ret.mp_ports_ret_t_u.ports, 0, STORAGE_NODE_PORTS_MAX * sizeof (uint32_t));
    
    if (!memcpy(&ret.mp_ports_ret_t_u.ports, ports, process_nb * sizeof (uint32_t))) {
        goto out;
    }

    /*
        for (i = 0; i < process_nb; i++) {

            memcpy(&ret.mp_ports_ret_t_u.ports[i], &ports[i], sizeof (uint32_t));

        }
     */

    ret.status = MP_SUCCESS;

out:
    return &ret;
}
