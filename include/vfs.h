#ifndef VFS_H
#define VFS_H

#include <stdint.h>

struct vnode {
    struct vfs_ops *ops;
    void *data;
    uint64_t size;
};

struct vfs_ops {
    int (*read)(struct vnode *vn, uint64_t offset, void *buf, size_t size);
    int (*write)(struct vnode *vn, uint64_t offset, void *buf, size_t size);
};

void vfs_init();
struct vnode *vfs_create_file(const char *name);
int vfs_read_file(const char *name, uint64_t offset, void *buf, size_t size);
int vfs_write_file(const char *name, uint64_t offset, void *buf, size_t size);

#endif