#include <stdint.h>
#include "fs.h"
#include "pmm.h"

struct fs root_fs;

static int names_equal(const char *a, const char *b) {
    while (*a && *b && *a == *b) {
        a++;
        b++;
    }
    return *a == *b;
}

static struct inode *find_inode(const char *name) {
    for (int i = 0; i < root_fs.num_inodes; i++) {
        if (names_equal(root_fs.inodes[i].name, name)) {
            return &root_fs.inodes[i];
        }
    }
    return 0;
}

void fs_init() {
    for (int i = 0; i < MAX_FILES; i++) {
        root_fs.inodes[i].name[0] = '\0';
        root_fs.inodes[i].type = 0;
        root_fs.inodes[i].size = 0;
        root_fs.inodes[i].data = 0;
    }
    root_fs.num_inodes = 0;
}

int fs_create_file(const char *name, uint64_t size) {
    if (find_inode(name) != 0) return -1;
    if (root_fs.num_inodes >= MAX_FILES) return -1;
    struct inode *inode = &root_fs.inodes[root_fs.num_inodes++];
    char *d = inode->name;
    while (*name) *d++ = *name++;
    *d = 0;
    inode->type = 0;
    if (size > 4096) {
        size = 4096;
    }
    inode->size = size;
    inode->data = (void *)pmm_alloc_page();
    if (inode->data == 0) {
        root_fs.num_inodes--;
        return -1;
    }
    return 0;
}

int fs_write_file(const char *name, void *data, uint64_t size) {
    struct inode *inode = find_inode(name);
    if (inode != 0) {
        if (size > inode->size) {
            size = inode->size;
        }
        const char *src = (const char *)data;
        char *dst = (char *)inode->data;
        for (uint64_t j = 0; j < size; j++) *dst++ = *src++;
        return 0;
    }
    return -1;
}

int fs_read_file(const char *name, void *buf, uint64_t size) {
    struct inode *inode = find_inode(name);
    if (inode != 0) {
        if (size > inode->size) {
            size = inode->size;
        }
        const char *src = (const char *)inode->data;
        char *dst = (char *)buf;
        for (uint64_t j = 0; j < size; j++) *dst++ = *src++;
        return 0;
    }
    return -1;
}
