#include <stddef.h>
#include "vfs.h"
#include "pmm.h"
#include "block.h"

#define MAX_VNODES 100
#define VNODE_NAME_MAX 64

struct ntfs_boot {
    uint8_t jump[3];
    uint8_t oem[8];
} __attribute__((packed));

struct vnode vnodes[MAX_VNODES];
static char vnode_names[MAX_VNODES][VNODE_NAME_MAX];
int num_vnodes;

static int name_equals(const char *a, const char *b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++;
        b++;
    }
    return *a == *b;
}

static void copy_name(char *dst, const char *src) {
    size_t i = 0;
    for (; i + 1 < VNODE_NAME_MAX && src[i] != '\0'; i++) {
        dst[i] = src[i];
    }
    dst[i] = '\0';
}

static struct vnode *find_vnode(const char *name) {
    for (int i = 0; i < num_vnodes; i++) {
        if (name_equals(vnode_names[i], name)) {
            return &vnodes[i];
        }
    }
    return NULL;
}

int ramfs_read(struct vnode *vn, uint64_t offset, void *buf, size_t size) {
    if (offset >= vn->size) return 0;
    if (offset + size > vn->size) size = vn->size - offset;
    __builtin_memcpy(buf, (uint8_t *)vn->data + offset, size);
    return size;
}

int ramfs_write(struct vnode *vn, uint64_t offset, void *buf, size_t size) {
    if (offset >= vn->size) {
        return 0;
    }
    if (offset + size > vn->size) {
        size = (size_t)(vn->size - offset);
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

    if (offset >= vn->size) return 0;
    if (offset + size > vn->size) size = (size_t)(vn->size - offset);

    __builtin_memcpy(buf, (uint8_t *)vn->data + offset, size);

    return size;

}

int ntfs_write(struct vnode *vn, uint64_t offset, void *buf, size_t size) {

    // NTFS write implementation

    // Update MFT, data runs, clusters

    // For now, simple

    if (offset >= vn->size) return 0;
    if (offset + size > vn->size) size = (size_t)(vn->size - offset);

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
    struct vnode *vn = &vnodes[num_vnodes];
    vn->ops = &ramfs_ops;
    vn->size = 4096;
    vn->data = (void *)pmm_alloc_page();
    if (!vn->data) return NULL;
    copy_name(vnode_names[num_vnodes], name);
    num_vnodes++;
    return vn;
}

int vfs_read_file(const char *name, uint64_t offset, void *buf, size_t size) {
    struct vnode *vn = find_vnode(name);
    if (vn && vn->ops && vn->ops->read) {
        return vn->ops->read(vn, offset, buf, size);
    }
    return 0;
}

int vfs_write_file(const char *name, uint64_t offset, void *buf, size_t size) {
    struct vnode *vn = find_vnode(name);
    if (vn && vn->ops && vn->ops->write) {
        return vn->ops->write(vn, offset, buf, size);
    }
    return 0;
}
