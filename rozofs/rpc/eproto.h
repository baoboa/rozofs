/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _EPROTO_H_RPCGEN
#define _EPROTO_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif

#include <rozofs/rozofs.h>

typedef uint32_t ep_uuid_t[ROZOFS_UUID_SIZE_NET];

typedef char *ep_name_t;

typedef char *ep_xattr_name_t;

typedef char *ep_xattr_value_t;

typedef u_char ep_xattr_list_t[ROZOFS_XATTR_LIST_MAX];

typedef char *ep_path_t;

typedef char *ep_link_t;

typedef char ep_host_t[ROZOFS_HOSTNAME_MAX];

typedef char ep_md5_t[ROZOFS_MD5_SIZE];

typedef char *ep_st_host_t;

typedef char *ep_epgw_host_t;

struct ep_gateway_t {
	uint32_t eid;
	uint32_t nb_gateways;
	uint32_t gateway_rank;
	uint32_t hash_config;
};
typedef struct ep_gateway_t ep_gateway_t;

enum ep_status_t {
	EP_SUCCESS = 0,
	EP_FAILURE = 1,
	EP_EMPTY = 2,
	EP_FAILURE_EID_NOT_SUPPORTED = 3,
	EP_NOT_SYNCED = 4,
	EP_EAGAIN = 4 + 1,
};
typedef enum ep_status_t ep_status_t;

struct ep_status_ret_t {
	ep_status_t status;
	union {
		uint64_t error;
	} ep_status_ret_t_u;
};
typedef struct ep_status_ret_t ep_status_ret_t;

struct epgw_status_ret_t {
	struct ep_gateway_t hdr;
	ep_status_ret_t status_gw;
};
typedef struct epgw_status_ret_t epgw_status_ret_t;

struct ep_storage_t {
	ep_host_t host;
	uint8_t sid;
};
typedef struct ep_storage_t ep_storage_t;

struct ep_cluster_t {
	uint16_t cid;
	uint8_t storages_nb;
	ep_storage_t storages[SID_MAX];
};
typedef struct ep_cluster_t ep_cluster_t;

struct ep_cluster_ret_t {
	ep_status_t status;
	union {
		ep_cluster_t cluster;
		int error;
	} ep_cluster_ret_t_u;
};
typedef struct ep_cluster_ret_t ep_cluster_ret_t;

struct epgw_cluster_ret_t {
	struct ep_gateway_t hdr;
	ep_cluster_ret_t status_gw;
};
typedef struct epgw_cluster_ret_t epgw_cluster_ret_t;

struct epgw_cluster_arg_t {
	struct ep_gateway_t hdr;
	int16_t cid;
};
typedef struct epgw_cluster_arg_t epgw_cluster_arg_t;

struct ep_storage_node_t {
	ep_host_t host;
	uint8_t sids_nb;
	uint8_t sids[STORAGES_MAX_BY_STORAGE_NODE];
	uint16_t cids[STORAGES_MAX_BY_STORAGE_NODE];
};
typedef struct ep_storage_node_t ep_storage_node_t;

struct ep_export_t {
	uint32_t hash_conf;
	uint32_t eid;
	uint32_t listen_port;
	ep_md5_t md5;
	ep_uuid_t rfid;
	uint8_t rl;
	uint32_t bs;
	uint8_t storage_nodes_nb;
	ep_storage_node_t storage_nodes[STORAGE_NODES_MAX];
};
typedef struct ep_export_t ep_export_t;

struct ep_mount_ret_t {
	ep_status_t status;
	union {
		ep_export_t export;
		int error;
	} ep_mount_ret_t_u;
};
typedef struct ep_mount_ret_t ep_mount_ret_t;

struct epgw_mount_ret_t {
	struct ep_gateway_t hdr;
	ep_mount_ret_t status_gw;
};
typedef struct epgw_mount_ret_t epgw_mount_ret_t;

struct ep_cnf_storage_node_t {
	char *host;
	uint8_t sids_nb;
	uint8_t sids[STORAGES_MAX_BY_STORAGE_NODE];
	uint16_t cids[STORAGES_MAX_BY_STORAGE_NODE];
};
typedef struct ep_cnf_storage_node_t ep_cnf_storage_node_t;

struct ep_conf_export_t {
	uint32_t hash_conf;
	uint32_t eid;
	ep_md5_t md5;
	ep_uuid_t rfid;
	uint8_t rl;
	struct {
		u_int storage_nodes_len;
		ep_cnf_storage_node_t *storage_nodes_val;
	} storage_nodes;
};
typedef struct ep_conf_export_t ep_conf_export_t;

struct ep_conf_ret_t {
	ep_status_t status;
	union {
		ep_conf_export_t export;
		int error;
	} ep_conf_ret_t_u;
};
typedef struct ep_conf_ret_t ep_conf_ret_t;

struct epgw_conf_ret_t {
	struct ep_gateway_t hdr;
	ep_conf_ret_t status_gw;
};
typedef struct epgw_conf_ret_t epgw_conf_ret_t;

struct ep_mattr_t {
	ep_uuid_t fid;
	uint16_t cid;
	uint8_t sids[ROZOFS_SAFE_MAX];
	uint32_t mode;
	uint32_t uid;
	uint32_t gid;
	uint16_t nlink;
	uint64_t ctime;
	uint64_t atime;
	uint64_t mtime;
	uint64_t size;
	uint32_t children;
};
typedef struct ep_mattr_t ep_mattr_t;

struct ep_mattr_ret_t {
	ep_status_t status;
	union {
		ep_mattr_t attrs;
		int error;
	} ep_mattr_ret_t_u;
};
typedef struct ep_mattr_ret_t ep_mattr_ret_t;

struct epgw_mattr_ret_t {
	struct ep_gateway_t hdr;
	uint64_t free_quota;
	uint32_t bsize;
	uint8_t layout;
	ep_mattr_ret_t status_gw;
	ep_mattr_ret_t parent_attr;
};
typedef struct epgw_mattr_ret_t epgw_mattr_ret_t;

struct ep_fid_ret_t {
	ep_status_t status;
	union {
		ep_uuid_t fid;
		int error;
	} ep_fid_ret_t_u;
};
typedef struct ep_fid_ret_t ep_fid_ret_t;

struct epgw_fid_ret_t {
	struct ep_gateway_t hdr;
	ep_fid_ret_t status_gw;
	ep_mattr_ret_t parent_attr;
};
typedef struct epgw_fid_ret_t epgw_fid_ret_t;

struct ep_lookup_arg_t {
	uint32_t eid;
	ep_uuid_t parent;
	ep_name_t name;
};
typedef struct ep_lookup_arg_t ep_lookup_arg_t;

struct epgw_lookup_arg_t {
	struct ep_gateway_t hdr;
	ep_lookup_arg_t arg_gw;
};
typedef struct epgw_lookup_arg_t epgw_lookup_arg_t;

enum ep_lock_mode_t {
	EP_LOCK_FREE = 0,
	EP_LOCK_READ = 1,
	EP_LOCK_WRITE = 2,
};
typedef enum ep_lock_mode_t ep_lock_mode_t;

enum ep_lock_size_t {
	EP_LOCK_NULL = 0,
	EP_LOCK_TOTAL = 0 + 1,
	EP_LOCK_FROM_START = 0 + 2,
	EP_LOCK_TO_END = 0 + 3,
	EP_LOCK_PARTIAL = 0 + 4,
};
typedef enum ep_lock_size_t ep_lock_size_t;

struct ep_lock_range_t {
	ep_lock_size_t size;
	uint64_t offset_start;
	uint64_t offset_stop;
};
typedef struct ep_lock_range_t ep_lock_range_t;

struct ep_lock_t {
	enum ep_lock_mode_t mode;
	uint64_t client_ref;
	uint64_t owner_ref;
	ep_lock_range_t user_range;
	ep_lock_range_t effective_range;
};
typedef struct ep_lock_t ep_lock_t;

struct ep_lock_request_arg_t {
	uint32_t eid;
	ep_uuid_t fid;
	ep_lock_t lock;
};
typedef struct ep_lock_request_arg_t ep_lock_request_arg_t;

struct epgw_lock_arg_t {
	struct ep_gateway_t hdr;
	struct ep_lock_request_arg_t arg_gw;
};
typedef struct epgw_lock_arg_t epgw_lock_arg_t;

struct ep_lock_ret_t {
	ep_status_t status;
	union {
		uint64_t error;
		struct ep_lock_t lock;
	} ep_lock_ret_t_u;
};
typedef struct ep_lock_ret_t ep_lock_ret_t;

struct epgw_lock_ret_t {
	struct ep_gateway_t hdr;
	ep_lock_ret_t gw_status;
};
typedef struct epgw_lock_ret_t epgw_lock_ret_t;

struct ep_mfile_arg_t {
	uint32_t eid;
	ep_uuid_t fid;
};
typedef struct ep_mfile_arg_t ep_mfile_arg_t;

struct epgw_mfile_arg_t {
	struct ep_gateway_t hdr;
	ep_mfile_arg_t arg_gw;
};
typedef struct epgw_mfile_arg_t epgw_mfile_arg_t;

struct ep_unlink_arg_t {
	uint32_t eid;
	ep_uuid_t pfid;
	ep_name_t name;
};
typedef struct ep_unlink_arg_t ep_unlink_arg_t;

struct epgw_unlink_arg_t {
	struct ep_gateway_t hdr;
	ep_unlink_arg_t arg_gw;
};
typedef struct epgw_unlink_arg_t epgw_unlink_arg_t;

struct ep_rmdir_arg_t {
	uint32_t eid;
	ep_uuid_t pfid;
	ep_name_t name;
};
typedef struct ep_rmdir_arg_t ep_rmdir_arg_t;

struct epgw_rmdir_arg_t {
	struct ep_gateway_t hdr;
	ep_rmdir_arg_t arg_gw;
};
typedef struct epgw_rmdir_arg_t epgw_rmdir_arg_t;

struct ep_statfs_t {
	uint32_t bsize;
	uint64_t blocks;
	uint64_t bfree;
	uint64_t files;
	uint64_t ffree;
	uint16_t namemax;
};
typedef struct ep_statfs_t ep_statfs_t;

struct ep_statfs_ret_t {
	ep_status_t status;
	union {
		ep_statfs_t stat;
		int error;
	} ep_statfs_ret_t_u;
};
typedef struct ep_statfs_ret_t ep_statfs_ret_t;

struct epgw_statfs_ret_t {
	struct ep_gateway_t hdr;
	ep_statfs_ret_t status_gw;
};
typedef struct epgw_statfs_ret_t epgw_statfs_ret_t;

struct ep_setattr_arg_t {
	uint32_t eid;
	uint32_t to_set;
	ep_mattr_t attrs;
};
typedef struct ep_setattr_arg_t ep_setattr_arg_t;

struct epgw_setattr_arg_t {
	struct ep_gateway_t hdr;
	ep_setattr_arg_t arg_gw;
};
typedef struct epgw_setattr_arg_t epgw_setattr_arg_t;

struct ep_getattr_ret_t {
	ep_status_t status;
	union {
		ep_mattr_t attrs;
		int error;
	} ep_getattr_ret_t_u;
};
typedef struct ep_getattr_ret_t ep_getattr_ret_t;

struct ep_readlink_ret_t {
	ep_status_t status;
	union {
		ep_link_t link;
		int error;
	} ep_readlink_ret_t_u;
};
typedef struct ep_readlink_ret_t ep_readlink_ret_t;

struct epgw_readlink_ret_t {
	struct ep_gateway_t hdr;
	ep_readlink_ret_t status_gw;
};
typedef struct epgw_readlink_ret_t epgw_readlink_ret_t;

struct ep_mknod_arg_t {
	uint32_t eid;
	ep_uuid_t parent;
	ep_name_t name;
	uint32_t uid;
	uint32_t gid;
	uint32_t mode;
};
typedef struct ep_mknod_arg_t ep_mknod_arg_t;

struct epgw_mknod_arg_t {
	struct ep_gateway_t hdr;
	ep_mknod_arg_t arg_gw;
};
typedef struct epgw_mknod_arg_t epgw_mknod_arg_t;

struct ep_link_arg_t {
	uint32_t eid;
	ep_uuid_t inode;
	ep_uuid_t newparent;
	ep_name_t newname;
};
typedef struct ep_link_arg_t ep_link_arg_t;

struct epgw_link_arg_t {
	struct ep_gateway_t hdr;
	ep_link_arg_t arg_gw;
};
typedef struct epgw_link_arg_t epgw_link_arg_t;

struct ep_mkdir_arg_t {
	uint32_t eid;
	ep_uuid_t parent;
	ep_name_t name;
	uint32_t uid;
	uint32_t gid;
	uint32_t mode;
};
typedef struct ep_mkdir_arg_t ep_mkdir_arg_t;

struct epgw_mkdir_arg_t {
	struct ep_gateway_t hdr;
	ep_mkdir_arg_t arg_gw;
};
typedef struct epgw_mkdir_arg_t epgw_mkdir_arg_t;

struct ep_symlink_arg_t {
	uint32_t eid;
	ep_link_t link;
	ep_uuid_t parent;
	ep_name_t name;
};
typedef struct ep_symlink_arg_t ep_symlink_arg_t;

struct epgw_symlink_arg_t {
	struct ep_gateway_t hdr;
	ep_symlink_arg_t arg_gw;
};
typedef struct epgw_symlink_arg_t epgw_symlink_arg_t;

typedef struct ep_child_t *ep_children_t;

struct ep_child_t {
	ep_name_t name;
	ep_uuid_t fid;
	ep_children_t next;
};
typedef struct ep_child_t ep_child_t;

struct dirlist_t {
	ep_children_t children;
	uint8_t eof;
	uint64_t cookie;
};
typedef struct dirlist_t dirlist_t;

struct ep_readdir_arg_t {
	uint32_t eid;
	ep_uuid_t fid;
	uint64_t cookie;
};
typedef struct ep_readdir_arg_t ep_readdir_arg_t;

struct epgw_readdir_arg_t {
	struct ep_gateway_t hdr;
	ep_readdir_arg_t arg_gw;
};
typedef struct epgw_readdir_arg_t epgw_readdir_arg_t;

struct ep_readdir_ret_t {
	ep_status_t status;
	union {
		dirlist_t reply;
		int error;
	} ep_readdir_ret_t_u;
};
typedef struct ep_readdir_ret_t ep_readdir_ret_t;

struct epgw_readdir_ret_t {
	struct ep_gateway_t hdr;
	ep_readdir_ret_t status_gw;
};
typedef struct epgw_readdir_ret_t epgw_readdir_ret_t;

struct ep_rename_arg_t {
	uint32_t eid;
	ep_uuid_t pfid;
	ep_name_t name;
	ep_uuid_t npfid;
	ep_name_t newname;
};
typedef struct ep_rename_arg_t ep_rename_arg_t;

struct epgw_rename_arg_t {
	struct ep_gateway_t hdr;
	ep_rename_arg_t arg_gw;
};
typedef struct epgw_rename_arg_t epgw_rename_arg_t;

struct epgw_rename_ret_t {
	struct ep_gateway_t hdr;
	ep_fid_ret_t status_gw;
	ep_mattr_ret_t child_attr;
	ep_mattr_ret_t parent_attr;
};
typedef struct epgw_rename_ret_t epgw_rename_ret_t;

struct ep_io_arg_t {
	uint32_t eid;
	ep_uuid_t fid;
	uint64_t offset;
	uint32_t length;
};
typedef struct ep_io_arg_t ep_io_arg_t;

struct epgw_io_arg_t {
	struct ep_gateway_t hdr;
	ep_io_arg_t arg_gw;
};
typedef struct epgw_io_arg_t epgw_io_arg_t;

struct ep_write_block_arg_t {
	uint32_t eid;
	ep_uuid_t fid;
	uint64_t bid;
	uint32_t nrb;
	uint16_t dist;
	uint64_t offset;
	uint32_t length;
	uint64_t geo_wr_start;
	uint64_t geo_wr_end;
};
typedef struct ep_write_block_arg_t ep_write_block_arg_t;

struct epgw_write_block_arg_t {
	struct ep_gateway_t hdr;
	ep_write_block_arg_t arg_gw;
};
typedef struct epgw_write_block_arg_t epgw_write_block_arg_t;

struct ep_read_t {
	struct {
		u_int dist_len;
		uint16_t *dist_val;
	} dist;
	int64_t length;
};
typedef struct ep_read_t ep_read_t;

struct ep_read_block_ret_t {
	ep_status_t status;
	union {
		ep_read_t ret;
		int error;
	} ep_read_block_ret_t_u;
};
typedef struct ep_read_block_ret_t ep_read_block_ret_t;

struct epgw_read_block_ret_t {
	struct ep_gateway_t hdr;
	ep_read_block_ret_t status_gw;
};
typedef struct epgw_read_block_ret_t epgw_read_block_ret_t;

struct ep_io_ret_t {
	ep_status_t status;
	union {
		int64_t length;
		int error;
	} ep_io_ret_t_u;
};
typedef struct ep_io_ret_t ep_io_ret_t;

struct epgw_io_ret_t {
	struct ep_gateway_t hdr;
	ep_io_ret_t status_gw;
	ep_mattr_ret_t attr;
};
typedef struct epgw_io_ret_t epgw_io_ret_t;

struct ep_setxattr_arg_t {
	uint32_t eid;
	ep_uuid_t fid;
	ep_xattr_name_t name;
	struct {
		u_int value_len;
		char *value_val;
	} value;
	uint8_t flags;
};
typedef struct ep_setxattr_arg_t ep_setxattr_arg_t;

struct epgw_setxattr_arg_t {
	struct ep_gateway_t hdr;
	ep_setxattr_arg_t arg_gw;
};
typedef struct epgw_setxattr_arg_t epgw_setxattr_arg_t;

struct ep_getxattr_arg_t {
	uint32_t eid;
	ep_uuid_t fid;
	ep_xattr_name_t name;
	uint64_t size;
};
typedef struct ep_getxattr_arg_t ep_getxattr_arg_t;

struct epgw_getxattr_arg_t {
	struct ep_gateway_t hdr;
	ep_getxattr_arg_t arg_gw;
};
typedef struct epgw_getxattr_arg_t epgw_getxattr_arg_t;

struct ep_getxattr_t {
	ep_xattr_value_t value;
	uint64_t size;
};
typedef struct ep_getxattr_t ep_getxattr_t;

struct ep_getxattr_ret_t {
	ep_status_t status;
	union {
		struct {
			u_int value_len;
			char *value_val;
		} value;
		int error;
	} ep_getxattr_ret_t_u;
};
typedef struct ep_getxattr_ret_t ep_getxattr_ret_t;

struct epgw_getxattr_ret_t {
	struct ep_gateway_t hdr;
	ep_getxattr_ret_t status_gw;
};
typedef struct epgw_getxattr_ret_t epgw_getxattr_ret_t;

struct ep_removexattr_arg_t {
	uint32_t eid;
	ep_uuid_t fid;
	ep_xattr_name_t name;
};
typedef struct ep_removexattr_arg_t ep_removexattr_arg_t;

struct epgw_removexattr_arg_t {
	struct ep_gateway_t hdr;
	ep_removexattr_arg_t arg_gw;
};
typedef struct epgw_removexattr_arg_t epgw_removexattr_arg_t;

struct ep_listxattr_arg_t {
	uint32_t eid;
	ep_uuid_t fid;
	uint64_t size;
};
typedef struct ep_listxattr_arg_t ep_listxattr_arg_t;

struct epgw_listxattr_arg_t {
	struct ep_gateway_t hdr;
	ep_listxattr_arg_t arg_gw;
};
typedef struct epgw_listxattr_arg_t epgw_listxattr_arg_t;

struct ep_listxattr_ret_t {
	ep_status_t status;
	union {
		struct {
			u_int list_len;
			char *list_val;
		} list;
		int error;
	} ep_listxattr_ret_t_u;
};
typedef struct ep_listxattr_ret_t ep_listxattr_ret_t;

struct epgw_listxattr_ret_t {
	struct ep_gateway_t hdr;
	ep_listxattr_ret_t status_gw;
};
typedef struct epgw_listxattr_ret_t epgw_listxattr_ret_t;

struct ep_gw_host_conf_t {
	ep_epgw_host_t host;
};
typedef struct ep_gw_host_conf_t ep_gw_host_conf_t;

struct ep_gw_header_t {
	uint32_t export_id;
	uint32_t nb_gateways;
	uint32_t gateway_rank;
	uint32_t configuration_indice;
};
typedef struct ep_gw_header_t ep_gw_header_t;

struct ep_gateway_configuration_t {
	ep_gw_header_t hdr;
	ep_epgw_host_t exportd_host;
	uint16_t exportd_port;
	uint16_t gateway_port;
	struct {
		u_int eid_len;
		uint32_t *eid_val;
	} eid;
	struct {
		u_int gateway_host_len;
		ep_gw_host_conf_t *gateway_host_val;
	} gateway_host;
};
typedef struct ep_gateway_configuration_t ep_gateway_configuration_t;

struct ep_gateway_configuration_ret_t {
	ep_status_t status;
	union {
		ep_gateway_configuration_t config;
		int error;
	} ep_gateway_configuration_ret_t_u;
};
typedef struct ep_gateway_configuration_ret_t ep_gateway_configuration_ret_t;

struct ep_gw_gateway_configuration_ret_t {
	struct ep_gateway_t hdr;
	ep_gateway_configuration_ret_t status_gw;
};
typedef struct ep_gw_gateway_configuration_ret_t ep_gw_gateway_configuration_ret_t;

struct epgw_mount_arg_t {
	struct ep_gateway_t hdr;
	ep_path_t path;
};
typedef struct epgw_mount_arg_t epgw_mount_arg_t;

struct epgw_conf_stor_arg_t {
	struct ep_gateway_t hdr;
	ep_path_t path;
};
typedef struct epgw_conf_stor_arg_t epgw_conf_stor_arg_t;

#define EXPORT_PROGRAM 0x20000001
#define EXPORT_VERSION 1

#if defined(__STDC__) || defined(__cplusplus)
#define EP_NULL 0
extern  void * ep_null_1(void *, CLIENT *);
extern  void * ep_null_1_svc(void *, struct svc_req *);
#define EP_MOUNT 1
extern  epgw_mount_ret_t * ep_mount_1(epgw_mount_arg_t *, CLIENT *);
extern  epgw_mount_ret_t * ep_mount_1_svc(epgw_mount_arg_t *, struct svc_req *);
#define EP_UMOUNT 2
extern  epgw_status_ret_t * ep_umount_1(uint32_t *, CLIENT *);
extern  epgw_status_ret_t * ep_umount_1_svc(uint32_t *, struct svc_req *);
#define EP_STATFS 3
extern  epgw_statfs_ret_t * ep_statfs_1(uint32_t *, CLIENT *);
extern  epgw_statfs_ret_t * ep_statfs_1_svc(uint32_t *, struct svc_req *);
#define EP_LOOKUP 4
extern  epgw_mattr_ret_t * ep_lookup_1(epgw_lookup_arg_t *, CLIENT *);
extern  epgw_mattr_ret_t * ep_lookup_1_svc(epgw_lookup_arg_t *, struct svc_req *);
#define EP_GETATTR 5
extern  epgw_mattr_ret_t * ep_getattr_1(epgw_mfile_arg_t *, CLIENT *);
extern  epgw_mattr_ret_t * ep_getattr_1_svc(epgw_mfile_arg_t *, struct svc_req *);
#define EP_SETATTR 6
extern  epgw_mattr_ret_t * ep_setattr_1(epgw_setattr_arg_t *, CLIENT *);
extern  epgw_mattr_ret_t * ep_setattr_1_svc(epgw_setattr_arg_t *, struct svc_req *);
#define EP_READLINK 7
extern  epgw_readlink_ret_t * ep_readlink_1(epgw_mfile_arg_t *, CLIENT *);
extern  epgw_readlink_ret_t * ep_readlink_1_svc(epgw_mfile_arg_t *, struct svc_req *);
#define EP_MKNOD 8
extern  epgw_mattr_ret_t * ep_mknod_1(epgw_mknod_arg_t *, CLIENT *);
extern  epgw_mattr_ret_t * ep_mknod_1_svc(epgw_mknod_arg_t *, struct svc_req *);
#define EP_MKDIR 9
extern  epgw_mattr_ret_t * ep_mkdir_1(epgw_mkdir_arg_t *, CLIENT *);
extern  epgw_mattr_ret_t * ep_mkdir_1_svc(epgw_mkdir_arg_t *, struct svc_req *);
#define EP_UNLINK 10
extern  epgw_fid_ret_t * ep_unlink_1(epgw_unlink_arg_t *, CLIENT *);
extern  epgw_fid_ret_t * ep_unlink_1_svc(epgw_unlink_arg_t *, struct svc_req *);
#define EP_RMDIR 12
extern  epgw_fid_ret_t * ep_rmdir_1(epgw_rmdir_arg_t *, CLIENT *);
extern  epgw_fid_ret_t * ep_rmdir_1_svc(epgw_rmdir_arg_t *, struct svc_req *);
#define EP_SYMLINK 13
extern  epgw_mattr_ret_t * ep_symlink_1(epgw_symlink_arg_t *, CLIENT *);
extern  epgw_mattr_ret_t * ep_symlink_1_svc(epgw_symlink_arg_t *, struct svc_req *);
#define EP_RENAME 14
extern  epgw_rename_ret_t * ep_rename_1(epgw_rename_arg_t *, CLIENT *);
extern  epgw_rename_ret_t * ep_rename_1_svc(epgw_rename_arg_t *, struct svc_req *);
#define EP_READDIR 15
extern  epgw_readdir_ret_t * ep_readdir_1(epgw_readdir_arg_t *, CLIENT *);
extern  epgw_readdir_ret_t * ep_readdir_1_svc(epgw_readdir_arg_t *, struct svc_req *);
#define EP_READ_BLOCK 16
extern  epgw_read_block_ret_t * ep_read_block_1(epgw_io_arg_t *, CLIENT *);
extern  epgw_read_block_ret_t * ep_read_block_1_svc(epgw_io_arg_t *, struct svc_req *);
#define EP_WRITE_BLOCK 17
extern  epgw_mattr_ret_t * ep_write_block_1(epgw_write_block_arg_t *, CLIENT *);
extern  epgw_mattr_ret_t * ep_write_block_1_svc(epgw_write_block_arg_t *, struct svc_req *);
#define EP_LINK 18
extern  epgw_mattr_ret_t * ep_link_1(epgw_link_arg_t *, CLIENT *);
extern  epgw_mattr_ret_t * ep_link_1_svc(epgw_link_arg_t *, struct svc_req *);
#define EP_SETXATTR 19
extern  epgw_status_ret_t * ep_setxattr_1(epgw_setxattr_arg_t *, CLIENT *);
extern  epgw_status_ret_t * ep_setxattr_1_svc(epgw_setxattr_arg_t *, struct svc_req *);
#define EP_GETXATTR 20
extern  epgw_getxattr_ret_t * ep_getxattr_1(epgw_getxattr_arg_t *, CLIENT *);
extern  epgw_getxattr_ret_t * ep_getxattr_1_svc(epgw_getxattr_arg_t *, struct svc_req *);
#define EP_REMOVEXATTR 21
extern  epgw_status_ret_t * ep_removexattr_1(epgw_removexattr_arg_t *, CLIENT *);
extern  epgw_status_ret_t * ep_removexattr_1_svc(epgw_removexattr_arg_t *, struct svc_req *);
#define EP_LISTXATTR 22
extern  epgw_listxattr_ret_t * ep_listxattr_1(epgw_listxattr_arg_t *, CLIENT *);
extern  epgw_listxattr_ret_t * ep_listxattr_1_svc(epgw_listxattr_arg_t *, struct svc_req *);
#define EP_LIST_CLUSTER 23
extern  epgw_cluster_ret_t * ep_list_cluster_1(epgw_cluster_arg_t *, CLIENT *);
extern  epgw_cluster_ret_t * ep_list_cluster_1_svc(epgw_cluster_arg_t *, struct svc_req *);
#define EP_CONF_STORAGE 24
extern  epgw_conf_ret_t * ep_conf_storage_1(epgw_conf_stor_arg_t *, CLIENT *);
extern  epgw_conf_ret_t * ep_conf_storage_1_svc(epgw_conf_stor_arg_t *, struct svc_req *);
#define EP_POLL_CONF 25
extern  epgw_status_ret_t * ep_poll_conf_1(ep_gateway_t *, CLIENT *);
extern  epgw_status_ret_t * ep_poll_conf_1_svc(ep_gateway_t *, struct svc_req *);
#define EP_CONF_EXPGW 26
extern  ep_gw_gateway_configuration_ret_t * ep_conf_expgw_1(ep_path_t *, CLIENT *);
extern  ep_gw_gateway_configuration_ret_t * ep_conf_expgw_1_svc(ep_path_t *, struct svc_req *);
#define EP_SET_FILE_LOCK 27
extern  epgw_lock_ret_t * ep_set_file_lock_1(epgw_lock_arg_t *, CLIENT *);
extern  epgw_lock_ret_t * ep_set_file_lock_1_svc(epgw_lock_arg_t *, struct svc_req *);
#define EP_GET_FILE_LOCK 28
extern  epgw_lock_ret_t * ep_get_file_lock_1(epgw_lock_arg_t *, CLIENT *);
extern  epgw_lock_ret_t * ep_get_file_lock_1_svc(epgw_lock_arg_t *, struct svc_req *);
#define EP_CLEAR_OWNER_FILE_LOCK 29
extern  epgw_status_ret_t * ep_clear_owner_file_lock_1(epgw_lock_arg_t *, CLIENT *);
extern  epgw_status_ret_t * ep_clear_owner_file_lock_1_svc(epgw_lock_arg_t *, struct svc_req *);
#define EP_CLEAR_CLIENT_FILE_LOCK 30
extern  epgw_status_ret_t * ep_clear_client_file_lock_1(epgw_lock_arg_t *, CLIENT *);
extern  epgw_status_ret_t * ep_clear_client_file_lock_1_svc(epgw_lock_arg_t *, struct svc_req *);
#define EP_POLL_FILE_LOCK 31
extern  epgw_status_ret_t * ep_poll_file_lock_1(epgw_lock_arg_t *, CLIENT *);
extern  epgw_status_ret_t * ep_poll_file_lock_1_svc(epgw_lock_arg_t *, struct svc_req *);
#define EP_GEO_POLL 32
extern  void * ep_geo_poll_1(void *, CLIENT *);
extern  void * ep_geo_poll_1_svc(void *, struct svc_req *);
extern int export_program_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);

#else /* K&R C */
#define EP_NULL 0
extern  void * ep_null_1();
extern  void * ep_null_1_svc();
#define EP_MOUNT 1
extern  epgw_mount_ret_t * ep_mount_1();
extern  epgw_mount_ret_t * ep_mount_1_svc();
#define EP_UMOUNT 2
extern  epgw_status_ret_t * ep_umount_1();
extern  epgw_status_ret_t * ep_umount_1_svc();
#define EP_STATFS 3
extern  epgw_statfs_ret_t * ep_statfs_1();
extern  epgw_statfs_ret_t * ep_statfs_1_svc();
#define EP_LOOKUP 4
extern  epgw_mattr_ret_t * ep_lookup_1();
extern  epgw_mattr_ret_t * ep_lookup_1_svc();
#define EP_GETATTR 5
extern  epgw_mattr_ret_t * ep_getattr_1();
extern  epgw_mattr_ret_t * ep_getattr_1_svc();
#define EP_SETATTR 6
extern  epgw_mattr_ret_t * ep_setattr_1();
extern  epgw_mattr_ret_t * ep_setattr_1_svc();
#define EP_READLINK 7
extern  epgw_readlink_ret_t * ep_readlink_1();
extern  epgw_readlink_ret_t * ep_readlink_1_svc();
#define EP_MKNOD 8
extern  epgw_mattr_ret_t * ep_mknod_1();
extern  epgw_mattr_ret_t * ep_mknod_1_svc();
#define EP_MKDIR 9
extern  epgw_mattr_ret_t * ep_mkdir_1();
extern  epgw_mattr_ret_t * ep_mkdir_1_svc();
#define EP_UNLINK 10
extern  epgw_fid_ret_t * ep_unlink_1();
extern  epgw_fid_ret_t * ep_unlink_1_svc();
#define EP_RMDIR 12
extern  epgw_fid_ret_t * ep_rmdir_1();
extern  epgw_fid_ret_t * ep_rmdir_1_svc();
#define EP_SYMLINK 13
extern  epgw_mattr_ret_t * ep_symlink_1();
extern  epgw_mattr_ret_t * ep_symlink_1_svc();
#define EP_RENAME 14
extern  epgw_rename_ret_t * ep_rename_1();
extern  epgw_rename_ret_t * ep_rename_1_svc();
#define EP_READDIR 15
extern  epgw_readdir_ret_t * ep_readdir_1();
extern  epgw_readdir_ret_t * ep_readdir_1_svc();
#define EP_READ_BLOCK 16
extern  epgw_read_block_ret_t * ep_read_block_1();
extern  epgw_read_block_ret_t * ep_read_block_1_svc();
#define EP_WRITE_BLOCK 17
extern  epgw_mattr_ret_t * ep_write_block_1();
extern  epgw_mattr_ret_t * ep_write_block_1_svc();
#define EP_LINK 18
extern  epgw_mattr_ret_t * ep_link_1();
extern  epgw_mattr_ret_t * ep_link_1_svc();
#define EP_SETXATTR 19
extern  epgw_status_ret_t * ep_setxattr_1();
extern  epgw_status_ret_t * ep_setxattr_1_svc();
#define EP_GETXATTR 20
extern  epgw_getxattr_ret_t * ep_getxattr_1();
extern  epgw_getxattr_ret_t * ep_getxattr_1_svc();
#define EP_REMOVEXATTR 21
extern  epgw_status_ret_t * ep_removexattr_1();
extern  epgw_status_ret_t * ep_removexattr_1_svc();
#define EP_LISTXATTR 22
extern  epgw_listxattr_ret_t * ep_listxattr_1();
extern  epgw_listxattr_ret_t * ep_listxattr_1_svc();
#define EP_LIST_CLUSTER 23
extern  epgw_cluster_ret_t * ep_list_cluster_1();
extern  epgw_cluster_ret_t * ep_list_cluster_1_svc();
#define EP_CONF_STORAGE 24
extern  epgw_conf_ret_t * ep_conf_storage_1();
extern  epgw_conf_ret_t * ep_conf_storage_1_svc();
#define EP_POLL_CONF 25
extern  epgw_status_ret_t * ep_poll_conf_1();
extern  epgw_status_ret_t * ep_poll_conf_1_svc();
#define EP_CONF_EXPGW 26
extern  ep_gw_gateway_configuration_ret_t * ep_conf_expgw_1();
extern  ep_gw_gateway_configuration_ret_t * ep_conf_expgw_1_svc();
#define EP_SET_FILE_LOCK 27
extern  epgw_lock_ret_t * ep_set_file_lock_1();
extern  epgw_lock_ret_t * ep_set_file_lock_1_svc();
#define EP_GET_FILE_LOCK 28
extern  epgw_lock_ret_t * ep_get_file_lock_1();
extern  epgw_lock_ret_t * ep_get_file_lock_1_svc();
#define EP_CLEAR_OWNER_FILE_LOCK 29
extern  epgw_status_ret_t * ep_clear_owner_file_lock_1();
extern  epgw_status_ret_t * ep_clear_owner_file_lock_1_svc();
#define EP_CLEAR_CLIENT_FILE_LOCK 30
extern  epgw_status_ret_t * ep_clear_client_file_lock_1();
extern  epgw_status_ret_t * ep_clear_client_file_lock_1_svc();
#define EP_POLL_FILE_LOCK 31
extern  epgw_status_ret_t * ep_poll_file_lock_1();
extern  epgw_status_ret_t * ep_poll_file_lock_1_svc();
#define EP_GEO_POLL 32
extern  void * ep_geo_poll_1();
extern  void * ep_geo_poll_1_svc();
extern int export_program_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_ep_uuid_t (XDR *, ep_uuid_t);
extern  bool_t xdr_ep_name_t (XDR *, ep_name_t*);
extern  bool_t xdr_ep_xattr_name_t (XDR *, ep_xattr_name_t*);
extern  bool_t xdr_ep_xattr_value_t (XDR *, ep_xattr_value_t*);
extern  bool_t xdr_ep_xattr_list_t (XDR *, ep_xattr_list_t);
extern  bool_t xdr_ep_path_t (XDR *, ep_path_t*);
extern  bool_t xdr_ep_link_t (XDR *, ep_link_t*);
extern  bool_t xdr_ep_host_t (XDR *, ep_host_t);
extern  bool_t xdr_ep_md5_t (XDR *, ep_md5_t);
extern  bool_t xdr_ep_st_host_t (XDR *, ep_st_host_t*);
extern  bool_t xdr_ep_epgw_host_t (XDR *, ep_epgw_host_t*);
extern  bool_t xdr_ep_gateway_t (XDR *, ep_gateway_t*);
extern  bool_t xdr_ep_status_t (XDR *, ep_status_t*);
extern  bool_t xdr_ep_status_ret_t (XDR *, ep_status_ret_t*);
extern  bool_t xdr_epgw_status_ret_t (XDR *, epgw_status_ret_t*);
extern  bool_t xdr_ep_storage_t (XDR *, ep_storage_t*);
extern  bool_t xdr_ep_cluster_t (XDR *, ep_cluster_t*);
extern  bool_t xdr_ep_cluster_ret_t (XDR *, ep_cluster_ret_t*);
extern  bool_t xdr_epgw_cluster_ret_t (XDR *, epgw_cluster_ret_t*);
extern  bool_t xdr_epgw_cluster_arg_t (XDR *, epgw_cluster_arg_t*);
extern  bool_t xdr_ep_storage_node_t (XDR *, ep_storage_node_t*);
extern  bool_t xdr_ep_export_t (XDR *, ep_export_t*);
extern  bool_t xdr_ep_mount_ret_t (XDR *, ep_mount_ret_t*);
extern  bool_t xdr_epgw_mount_ret_t (XDR *, epgw_mount_ret_t*);
extern  bool_t xdr_ep_cnf_storage_node_t (XDR *, ep_cnf_storage_node_t*);
extern  bool_t xdr_ep_conf_export_t (XDR *, ep_conf_export_t*);
extern  bool_t xdr_ep_conf_ret_t (XDR *, ep_conf_ret_t*);
extern  bool_t xdr_epgw_conf_ret_t (XDR *, epgw_conf_ret_t*);
extern  bool_t xdr_ep_mattr_t (XDR *, ep_mattr_t*);
extern  bool_t xdr_ep_mattr_ret_t (XDR *, ep_mattr_ret_t*);
extern  bool_t xdr_epgw_mattr_ret_t (XDR *, epgw_mattr_ret_t*);
extern  bool_t xdr_ep_fid_ret_t (XDR *, ep_fid_ret_t*);
extern  bool_t xdr_epgw_fid_ret_t (XDR *, epgw_fid_ret_t*);
extern  bool_t xdr_ep_lookup_arg_t (XDR *, ep_lookup_arg_t*);
extern  bool_t xdr_epgw_lookup_arg_t (XDR *, epgw_lookup_arg_t*);
extern  bool_t xdr_ep_lock_mode_t (XDR *, ep_lock_mode_t*);
extern  bool_t xdr_ep_lock_size_t (XDR *, ep_lock_size_t*);
extern  bool_t xdr_ep_lock_range_t (XDR *, ep_lock_range_t*);
extern  bool_t xdr_ep_lock_t (XDR *, ep_lock_t*);
extern  bool_t xdr_ep_lock_request_arg_t (XDR *, ep_lock_request_arg_t*);
extern  bool_t xdr_epgw_lock_arg_t (XDR *, epgw_lock_arg_t*);
extern  bool_t xdr_ep_lock_ret_t (XDR *, ep_lock_ret_t*);
extern  bool_t xdr_epgw_lock_ret_t (XDR *, epgw_lock_ret_t*);
extern  bool_t xdr_ep_mfile_arg_t (XDR *, ep_mfile_arg_t*);
extern  bool_t xdr_epgw_mfile_arg_t (XDR *, epgw_mfile_arg_t*);
extern  bool_t xdr_ep_unlink_arg_t (XDR *, ep_unlink_arg_t*);
extern  bool_t xdr_epgw_unlink_arg_t (XDR *, epgw_unlink_arg_t*);
extern  bool_t xdr_ep_rmdir_arg_t (XDR *, ep_rmdir_arg_t*);
extern  bool_t xdr_epgw_rmdir_arg_t (XDR *, epgw_rmdir_arg_t*);
extern  bool_t xdr_ep_statfs_t (XDR *, ep_statfs_t*);
extern  bool_t xdr_ep_statfs_ret_t (XDR *, ep_statfs_ret_t*);
extern  bool_t xdr_epgw_statfs_ret_t (XDR *, epgw_statfs_ret_t*);
extern  bool_t xdr_ep_setattr_arg_t (XDR *, ep_setattr_arg_t*);
extern  bool_t xdr_epgw_setattr_arg_t (XDR *, epgw_setattr_arg_t*);
extern  bool_t xdr_ep_getattr_ret_t (XDR *, ep_getattr_ret_t*);
extern  bool_t xdr_ep_readlink_ret_t (XDR *, ep_readlink_ret_t*);
extern  bool_t xdr_epgw_readlink_ret_t (XDR *, epgw_readlink_ret_t*);
extern  bool_t xdr_ep_mknod_arg_t (XDR *, ep_mknod_arg_t*);
extern  bool_t xdr_epgw_mknod_arg_t (XDR *, epgw_mknod_arg_t*);
extern  bool_t xdr_ep_link_arg_t (XDR *, ep_link_arg_t*);
extern  bool_t xdr_epgw_link_arg_t (XDR *, epgw_link_arg_t*);
extern  bool_t xdr_ep_mkdir_arg_t (XDR *, ep_mkdir_arg_t*);
extern  bool_t xdr_epgw_mkdir_arg_t (XDR *, epgw_mkdir_arg_t*);
extern  bool_t xdr_ep_symlink_arg_t (XDR *, ep_symlink_arg_t*);
extern  bool_t xdr_epgw_symlink_arg_t (XDR *, epgw_symlink_arg_t*);
extern  bool_t xdr_ep_children_t (XDR *, ep_children_t*);
extern  bool_t xdr_ep_child_t (XDR *, ep_child_t*);
extern  bool_t xdr_dirlist_t (XDR *, dirlist_t*);
extern  bool_t xdr_ep_readdir_arg_t (XDR *, ep_readdir_arg_t*);
extern  bool_t xdr_epgw_readdir_arg_t (XDR *, epgw_readdir_arg_t*);
extern  bool_t xdr_ep_readdir_ret_t (XDR *, ep_readdir_ret_t*);
extern  bool_t xdr_epgw_readdir_ret_t (XDR *, epgw_readdir_ret_t*);
extern  bool_t xdr_ep_rename_arg_t (XDR *, ep_rename_arg_t*);
extern  bool_t xdr_epgw_rename_arg_t (XDR *, epgw_rename_arg_t*);
extern  bool_t xdr_epgw_rename_ret_t (XDR *, epgw_rename_ret_t*);
extern  bool_t xdr_ep_io_arg_t (XDR *, ep_io_arg_t*);
extern  bool_t xdr_epgw_io_arg_t (XDR *, epgw_io_arg_t*);
extern  bool_t xdr_ep_write_block_arg_t (XDR *, ep_write_block_arg_t*);
extern  bool_t xdr_epgw_write_block_arg_t (XDR *, epgw_write_block_arg_t*);
extern  bool_t xdr_ep_read_t (XDR *, ep_read_t*);
extern  bool_t xdr_ep_read_block_ret_t (XDR *, ep_read_block_ret_t*);
extern  bool_t xdr_epgw_read_block_ret_t (XDR *, epgw_read_block_ret_t*);
extern  bool_t xdr_ep_io_ret_t (XDR *, ep_io_ret_t*);
extern  bool_t xdr_epgw_io_ret_t (XDR *, epgw_io_ret_t*);
extern  bool_t xdr_ep_setxattr_arg_t (XDR *, ep_setxattr_arg_t*);
extern  bool_t xdr_epgw_setxattr_arg_t (XDR *, epgw_setxattr_arg_t*);
extern  bool_t xdr_ep_getxattr_arg_t (XDR *, ep_getxattr_arg_t*);
extern  bool_t xdr_epgw_getxattr_arg_t (XDR *, epgw_getxattr_arg_t*);
extern  bool_t xdr_ep_getxattr_t (XDR *, ep_getxattr_t*);
extern  bool_t xdr_ep_getxattr_ret_t (XDR *, ep_getxattr_ret_t*);
extern  bool_t xdr_epgw_getxattr_ret_t (XDR *, epgw_getxattr_ret_t*);
extern  bool_t xdr_ep_removexattr_arg_t (XDR *, ep_removexattr_arg_t*);
extern  bool_t xdr_epgw_removexattr_arg_t (XDR *, epgw_removexattr_arg_t*);
extern  bool_t xdr_ep_listxattr_arg_t (XDR *, ep_listxattr_arg_t*);
extern  bool_t xdr_epgw_listxattr_arg_t (XDR *, epgw_listxattr_arg_t*);
extern  bool_t xdr_ep_listxattr_ret_t (XDR *, ep_listxattr_ret_t*);
extern  bool_t xdr_epgw_listxattr_ret_t (XDR *, epgw_listxattr_ret_t*);
extern  bool_t xdr_ep_gw_host_conf_t (XDR *, ep_gw_host_conf_t*);
extern  bool_t xdr_ep_gw_header_t (XDR *, ep_gw_header_t*);
extern  bool_t xdr_ep_gateway_configuration_t (XDR *, ep_gateway_configuration_t*);
extern  bool_t xdr_ep_gateway_configuration_ret_t (XDR *, ep_gateway_configuration_ret_t*);
extern  bool_t xdr_ep_gw_gateway_configuration_ret_t (XDR *, ep_gw_gateway_configuration_ret_t*);
extern  bool_t xdr_epgw_mount_arg_t (XDR *, epgw_mount_arg_t*);
extern  bool_t xdr_epgw_conf_stor_arg_t (XDR *, epgw_conf_stor_arg_t*);

#else /* K&R C */
extern bool_t xdr_ep_uuid_t ();
extern bool_t xdr_ep_name_t ();
extern bool_t xdr_ep_xattr_name_t ();
extern bool_t xdr_ep_xattr_value_t ();
extern bool_t xdr_ep_xattr_list_t ();
extern bool_t xdr_ep_path_t ();
extern bool_t xdr_ep_link_t ();
extern bool_t xdr_ep_host_t ();
extern bool_t xdr_ep_md5_t ();
extern bool_t xdr_ep_st_host_t ();
extern bool_t xdr_ep_epgw_host_t ();
extern bool_t xdr_ep_gateway_t ();
extern bool_t xdr_ep_status_t ();
extern bool_t xdr_ep_status_ret_t ();
extern bool_t xdr_epgw_status_ret_t ();
extern bool_t xdr_ep_storage_t ();
extern bool_t xdr_ep_cluster_t ();
extern bool_t xdr_ep_cluster_ret_t ();
extern bool_t xdr_epgw_cluster_ret_t ();
extern bool_t xdr_epgw_cluster_arg_t ();
extern bool_t xdr_ep_storage_node_t ();
extern bool_t xdr_ep_export_t ();
extern bool_t xdr_ep_mount_ret_t ();
extern bool_t xdr_epgw_mount_ret_t ();
extern bool_t xdr_ep_cnf_storage_node_t ();
extern bool_t xdr_ep_conf_export_t ();
extern bool_t xdr_ep_conf_ret_t ();
extern bool_t xdr_epgw_conf_ret_t ();
extern bool_t xdr_ep_mattr_t ();
extern bool_t xdr_ep_mattr_ret_t ();
extern bool_t xdr_epgw_mattr_ret_t ();
extern bool_t xdr_ep_fid_ret_t ();
extern bool_t xdr_epgw_fid_ret_t ();
extern bool_t xdr_ep_lookup_arg_t ();
extern bool_t xdr_epgw_lookup_arg_t ();
extern bool_t xdr_ep_lock_mode_t ();
extern bool_t xdr_ep_lock_size_t ();
extern bool_t xdr_ep_lock_range_t ();
extern bool_t xdr_ep_lock_t ();
extern bool_t xdr_ep_lock_request_arg_t ();
extern bool_t xdr_epgw_lock_arg_t ();
extern bool_t xdr_ep_lock_ret_t ();
extern bool_t xdr_epgw_lock_ret_t ();
extern bool_t xdr_ep_mfile_arg_t ();
extern bool_t xdr_epgw_mfile_arg_t ();
extern bool_t xdr_ep_unlink_arg_t ();
extern bool_t xdr_epgw_unlink_arg_t ();
extern bool_t xdr_ep_rmdir_arg_t ();
extern bool_t xdr_epgw_rmdir_arg_t ();
extern bool_t xdr_ep_statfs_t ();
extern bool_t xdr_ep_statfs_ret_t ();
extern bool_t xdr_epgw_statfs_ret_t ();
extern bool_t xdr_ep_setattr_arg_t ();
extern bool_t xdr_epgw_setattr_arg_t ();
extern bool_t xdr_ep_getattr_ret_t ();
extern bool_t xdr_ep_readlink_ret_t ();
extern bool_t xdr_epgw_readlink_ret_t ();
extern bool_t xdr_ep_mknod_arg_t ();
extern bool_t xdr_epgw_mknod_arg_t ();
extern bool_t xdr_ep_link_arg_t ();
extern bool_t xdr_epgw_link_arg_t ();
extern bool_t xdr_ep_mkdir_arg_t ();
extern bool_t xdr_epgw_mkdir_arg_t ();
extern bool_t xdr_ep_symlink_arg_t ();
extern bool_t xdr_epgw_symlink_arg_t ();
extern bool_t xdr_ep_children_t ();
extern bool_t xdr_ep_child_t ();
extern bool_t xdr_dirlist_t ();
extern bool_t xdr_ep_readdir_arg_t ();
extern bool_t xdr_epgw_readdir_arg_t ();
extern bool_t xdr_ep_readdir_ret_t ();
extern bool_t xdr_epgw_readdir_ret_t ();
extern bool_t xdr_ep_rename_arg_t ();
extern bool_t xdr_epgw_rename_arg_t ();
extern bool_t xdr_epgw_rename_ret_t ();
extern bool_t xdr_ep_io_arg_t ();
extern bool_t xdr_epgw_io_arg_t ();
extern bool_t xdr_ep_write_block_arg_t ();
extern bool_t xdr_epgw_write_block_arg_t ();
extern bool_t xdr_ep_read_t ();
extern bool_t xdr_ep_read_block_ret_t ();
extern bool_t xdr_epgw_read_block_ret_t ();
extern bool_t xdr_ep_io_ret_t ();
extern bool_t xdr_epgw_io_ret_t ();
extern bool_t xdr_ep_setxattr_arg_t ();
extern bool_t xdr_epgw_setxattr_arg_t ();
extern bool_t xdr_ep_getxattr_arg_t ();
extern bool_t xdr_epgw_getxattr_arg_t ();
extern bool_t xdr_ep_getxattr_t ();
extern bool_t xdr_ep_getxattr_ret_t ();
extern bool_t xdr_epgw_getxattr_ret_t ();
extern bool_t xdr_ep_removexattr_arg_t ();
extern bool_t xdr_epgw_removexattr_arg_t ();
extern bool_t xdr_ep_listxattr_arg_t ();
extern bool_t xdr_epgw_listxattr_arg_t ();
extern bool_t xdr_ep_listxattr_ret_t ();
extern bool_t xdr_epgw_listxattr_ret_t ();
extern bool_t xdr_ep_gw_host_conf_t ();
extern bool_t xdr_ep_gw_header_t ();
extern bool_t xdr_ep_gateway_configuration_t ();
extern bool_t xdr_ep_gateway_configuration_ret_t ();
extern bool_t xdr_ep_gw_gateway_configuration_ret_t ();
extern bool_t xdr_epgw_mount_arg_t ();
extern bool_t xdr_epgw_conf_stor_arg_t ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_EPROTO_H_RPCGEN */
