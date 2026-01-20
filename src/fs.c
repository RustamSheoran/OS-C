#include <stdint.h>
#include "fs.h"
#include "pmm.h"

struct fs root_fs;

void fs_init() {
    root_fs.num_inodes = 0;
}

int fs_create_file(const char *name, uint64_t size) {
    if (root_fs.num_inodes >= MAX_FILES) return -1;
    struct inode *inode = &root_fs.inodes[root_fs.num_inodes++];
    char *d = inode->name;
    while (*name) *d++ = *name++;
    *d = 0;
    inode->type = 0;
    inode->size = size;
    inode->data = (void *)pmm_alloc_page();
    return 0;
}

int fs_write_file(const char *name, void *data, uint64_t size) {
    for (int i = 0; i < root_fs.num_inodes; i++) {
        struct inode *inode = &root_fs.inodes[i];
        char *a = inode->name, *b = (char *)name;
        while (*a && *b && *a == *b) { a++; b++; }
        if (*a == *b) {
            char *src = data, *dst = inode->data;
            for (uint64_t j = 0; j < size; j++) *dst++ = *src++;
            return 0;
        }
    }
    return -1;
}

int fs_read_file(const char *name, void *buf, uint64_t size) {
    for (int i = 0; i < root_fs.num_inodes; i++) {
        struct inode *inode = &root_fs.inodes[i];
        char *a = inode->name, *b = (char *)name;
        while (*a && *b && *a == *b) { a++; b++; }
        if (*a == *b) {
            char *src = inode->data, *dst = buf;
            for (uint64_t j = 0; j < size; j++) *dst++ = *src++;
            return 0;
        }
    }
    return -1;
}