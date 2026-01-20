#include "vfs.h"
#include "pmm.h"

#define MAX_VNODES 100

struct vnode vnodes[MAX_VNODES];
int num_vnodes;

int ramfs_read(struct vnode *vn, uint64_t offset, void *buf, size_t size) {
    if (offset >= vn->size) return 0;
    if (offset + size > vn->size) size = vn->size - offset;
    memcpy(buf, (uint8_t *)vn->data + offset, size);
    return size;
}

int ramfs_write(struct vnode *vn, uint64_t offset, void *buf, size_t size) {
    if (offset + size > vn->size) return 0; // Stub
    memcpy((uint8_t *)vn->data + offset, buf, size);
    return size;
}

struct vfs_ops ramfs_ops = {
    .read = ramfs_read,
    .write = ramfs_write
};

void vfs_init() {
    num_vnodes = 0;
}

struct vnode *vfs_create_file(const char *name) {
    if (num_vnodes >= MAX_VNODES) return NULL;
    struct vnode *vn = &vnodes[num_vnodes++];
    vn->ops = &ramfs_ops;
    vn->size = 4096;
    vn->data = (void *)pmm_alloc_page();
    return vn;
}

int vfs_read_file(const char *name, uint64_t offset, void *buf, size_t size) {
    // Stub: use first vnode
    if (num_vnodes > 0) {
        return vnodes[0].ops->read(&vnodes[0], offset, buf, size);
    }
    return 0;
}

int vfs_write_file(const char *name, uint64_t offset, void *buf, size_t size) {
    if (num_vnodes > 0) {
        return vnodes[0].ops->write(&vnodes[0], offset, buf, size);
    }
    return 0;
}