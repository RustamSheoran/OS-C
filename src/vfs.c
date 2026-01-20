#include <stddef.h>
#include "vfs.h"
#include "pmm.h"
#include "block.h"

#define MAX_VNODES 100

struct vnode vnodes[MAX_VNODES];
int num_vnodes;

int ramfs_read(struct vnode *vn, uint64_t offset, void *buf, size_t size) {
    if (offset >= vn->size) return 0;
    if (offset + size > vn->size) size = vn->size - offset;
    __builtin_memcpy(buf, (uint8_t *)vn->data + offset, size);
    return size;
}

int ramfs_write(struct vnode *vn, uint64_t offset, void *buf, size_t size) {

    if (offset + size > vn->size) {

        // Extend, stub

        return 0;

    }

    __builtin_memcpy((uint8_t *)vn->data + offset, buf, size);

    return size;

}

int ntfs_read(struct vnode *vn, uint64_t offset, void *buf, size_t size) {

    // NTFS read implementation

    // 1. Read boot sector

    uint8_t sector[512];

    if (nvme_dev.read(0, sector) != 0) return 0;

    struct ntfs_boot *boot = (struct ntfs_boot *)sector;

    // 2. Validate NTFS

    if (boot->oem[0] != 'N' || boot->oem[1] != 'T' || boot->oem[2] != 'F' || boot->oem[3] != 'S') return 0;

    // 3. MFT parsing stub

    if (offset + size > vn->size) size = vn->size - offset;

    memcpy(buf, (uint8_t *)vn->data + offset, size);

    return size;

}

int ntfs_write(struct vnode *vn, uint64_t offset, void *buf, size_t size) {

    // NTFS write implementation

    // Update MFT, data runs, clusters

    // For now, simple

    if (offset + size > vn->size) return 0;

    __builtin_memcpy((uint8_t *)vn->data + offset, buf, size);

    return size;

}

struct vfs_ops ntfs_ops = {
    .read = ntfs_read,
    .write = ntfs_write
};

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