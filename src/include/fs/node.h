/**
 * @file
 * @brief pool of nodes
 *
 * @date 06.10.10
 * @author Nikolay Korotky
 */

#ifndef FS_NODE_H_
#define FS_NODE_H_

#include <sys/stat.h>
#include <fs/file_system.h>
#include <util/tree.h>
#include <limits.h>
#include <kernel/thread/sync/mutex.h>
#include <fcntl.h>


struct nas;

struct flock_shared {
	struct thread *holder;
	struct dlist_head flock_link;
};

struct node_flock {
	struct mutex      exlock;
	long              shlock_count;
	struct dlist_head shlock_holders;
	spinlock_t        flock_guard;
};

struct node {
	/* node name (use vfs_get_path_by_node() for get full path*/
	char                  name[NAME_MAX + 1];

	mode_t                mode;/* discrete access mode Read-Write-Execution */
	uid_t                 uid;/* owner user ID */
	gid_t                 gid;/* owner group ID */

	/* node attribute structure (extended information about node)*/
	struct nas            *nas;

	int                   mounted; /* is mount point*/

	struct node_flock     flock;

	/* service data structure for enabling tree operation */
	struct tree_link      tree_link;
};

struct node_info {
	size_t        size;
	unsigned int  ctime; /* time of last status change */
	unsigned int  mtime;
};

struct node_fi {
	struct node_info ni;
	void  *privdata;
};

typedef struct nas {
	struct node          *node;
	struct filesystem    *fs;
	struct node_fi       *fi;
} nas_t;

/**
 * @param name Non-empty string.
 * @param name_len (optional) how many bytes to take from name.
 *    If zero, the name must be a null-terminated string.
 */
extern struct node *node_alloc(const char *name, size_t name_len);

extern void node_free(struct node *node);

static inline struct node *node_parent(struct node *node) {
	return tree_element(node->tree_link.par, struct node, tree_link);
}

static inline int node_is_block_dev(struct node *node) {
	return S_ISBLK(node->mode);
}

static inline int node_is_directory(struct node *node) {
	return S_ISDIR(node->mode);
}

static inline int node_is_file(struct node *node) {
	return S_ISREG(node->mode);
}


#endif /* FS_NODE_H_ */
