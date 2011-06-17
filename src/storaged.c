/*
  Copyright (c) 2010 Fizians SAS. <http://www.fizians.com>
  This file is part of Rozo.

  Rozo is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published
  by the Free Software Foundation; either version 3 of the License,
  or (at your option) any later version.

  Rozo is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see
  <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <sys/stat.h>
#include <netinet/tcp.h>
#include <sys/resource.h>
#include <unistd.h>
#include <libintl.h>
#include <sys/poll.h>
#include <rpc/rpc.h>
#include <getopt.h>
#include <rpc/pmap_clnt.h>
#include <libconfig.h>

#include "config.h"
#include "xmalloc.h"
#include "log.h"
#include "list.h"
#include "daemon.h"
#include "storage.h"
#include "sproto.h"

#define STORAGED_PID_FILE "storaged.pid"

enum command {
    HELP,
    START,
    STOP
};

static char storaged_config_file[PATH_MAX] = STORAGED_DEFAULT_CONFIG;
static int storaged_command_flag = -1;
static storage_t *storaged_storages = 0;
static uint16_t storaged_nrstorages = 0;
extern void storage_program_1(struct svc_req *rqstp, SVCXPRT * ctl_svc);
static SVCXPRT *storaged_svc = 0;

storage_t *storaged_lookup(sid_t sid) {
    storage_t *st = 0;
    DEBUG_FUNCTION;

    st = storaged_storages;
    do {
        if (st->sid == sid)
            goto out;
    } while (st++ != storaged_storages + storaged_nrstorages);
    errno = EINVAL;
    st = 0;
out:
    return st;
}

static void storaged_initialize(rozo_layout_t layout, uint16_t nrstorages) {
    DEBUG_FUNCTION;
    rozo_initialize(layout);
    storaged_nrstorages = nrstorages;
    storaged_storages = xmalloc(nrstorages * sizeof (storage_t));
}

static void storaged_release() {
    DEBUG_FUNCTION;

    if (storaged_storages) {
        storage_t *st = storaged_storages;
        while (st != storaged_storages + storaged_nrstorages)
            storage_release(st++);
        free(storaged_storages);
        storaged_nrstorages = 0;
        storaged_storages = 0;
    }
}

static int configure() {
    int status = -1, i;
    struct config_t config;
    struct config_setting_t *settings = NULL;
    long int layout;
    DEBUG_FUNCTION;

    config_init(&config);
    if (config_read_file(&config, storaged_config_file) == CONFIG_FALSE)
        goto error;

    if (!config_lookup_int(&config, "layout", &layout))
        goto error;

    if (!(settings = config_lookup(&config, "storages")))
        goto error;

    storaged_initialize(layout, config_setting_length(settings));
    for (i = 0; i < config_setting_length(settings); i++) {
        struct config_setting_t *ms;
        long int sid;
        const char *root;
        if (!(ms = config_setting_get_elem(settings, i))) {
            goto error;
        }
        if (config_setting_lookup_int(ms, "sid", &sid) == CONFIG_FALSE) {
            goto error;
        }
        if (config_setting_lookup_string(ms, "root", &root) == CONFIG_FALSE) {
            goto error;
        }
        if (storage_initialize(storaged_storages + i, (uint16_t) sid, root) !=
            0) {
            goto out;
        }
    }

    status = 0;
    goto out;
error:
    fatal("Can't read config file:%s at line %d", config_error_text(&config),
          config_error_line(&config));
    errno = EIO;
out:
    config_destroy(&config);
    return status;
}

static void on_start() {
    int sock;
    int one = 1;
    DEBUG_FUNCTION;

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    setsockopt(sock, SOL_TCP, TCP_NODELAY, (void *) one, sizeof (one));
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void *) one, sizeof (one));

    if ((storaged_svc =
         svctcp_create(sock, ROZO_RPC_BUFFER_SIZE,
                       ROZO_RPC_BUFFER_SIZE)) == NULL) {
        fatal("can't create service.");
        return;
    }

    if (!svc_register
        (storaged_svc, STORAGE_PROGRAM, STORAGE_VERSION, storage_program_1,
         IPPROTO_TCP)) {
        fatal("can't register service : %s", strerror(errno));
        return;
    }

    info("running.");
    svc_run();
}

static void on_stop() {
    DEBUG_FUNCTION;

    svc_exit();
    svc_unregister(STORAGE_PROGRAM, STORAGE_VERSION);
    pmap_unset(STORAGE_PROGRAM, STORAGE_VERSION);
    if (storaged_svc) {
        svc_destroy(storaged_svc);
        storaged_svc = NULL;
    }
    storaged_release();
    rozo_release();

    info("stopped.");
}

void usage() {
    printf("Rozo storage daemon - %s\n", VERSION);
    printf
        ("Usage: storaged {--help} | {[{--config | -c} file] --start | --stop}\n\n");
    printf("\t-h, --help\tprint this message.\n");
    printf
        ("\t-c, --config\tconfig file to use (default *install prefix*/etc/rozo/storage.conf).\n");
    printf("\t--start\t\tstart the daemon.\n");
    printf("\t--stop\t\tstop the daemon\n");
    exit(EXIT_FAILURE);
};

int main(int argc, char *argv[]) {
    int c;
    static struct option long_options[] = {
        {"help", no_argument, &storaged_command_flag, HELP},
        {"start", no_argument, &storaged_command_flag, START},
        {"stop", no_argument, &storaged_command_flag, STOP},
        {"config", required_argument, 0, 'c'},
        {0, 0, 0, 0}
    };

    while (1) {
        int option_index = 0;
        c = getopt_long(argc, argv, "hc:", long_options, &option_index);

        if (c == -1)
            break;

        switch (c) {
        case 0:                //long option (manage by getopt but we don't want to be catched by default label)
            break;
        case 'h':
            storaged_command_flag = HELP;
            break;
        case 'c':
            if (!realpath(optarg, storaged_config_file)) {
                fprintf(stderr,
                        "storaged failed: configuration file: %s: %s\n",
                        optarg, strerror(errno));
                exit(EXIT_FAILURE);
            }
            break;
        case '?':
            exit(EXIT_FAILURE);
        default:
            exit(EXIT_FAILURE);
        }
    }

    switch (storaged_command_flag) {
    case HELP:
        usage();
        break;
    case START:
        openlog("storaged", LOG_PID, LOG_DAEMON);
        if (configure() != 0) {
            fprintf(stderr, "load config failed: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        daemon_start(STORAGED_PID_FILE, on_start, on_stop, NULL);
        break;
    case STOP:
        daemon_stop(STORAGED_PID_FILE);
        break;
    default:
        usage();
    }
    exit(0);
}
