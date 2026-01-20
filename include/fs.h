#ifndef FS_H
#define FS_H

#include <stdint.h>

#define MAX_FILES 100

struct inode {
    char name[256];
    uint8_t type; // 0 file, 1 dir
    uint64_t size;
    void *data;
};

struct fs {
    struct inode inodes[MAX_FILES];
    int num_inodes;
};

extern struct fs root_fs;

void fs_init();
int fs_create_file(const char *name, uint64_t size);
int fs_write_file(const char *name, void *data, uint64_t size);
int fs_read_file(const char *name, void *buf, uint64_t size);

#endif