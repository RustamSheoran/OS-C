#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>

typedef struct block_dev {
    int (*read)(uint64_t lba, uint8_t *buf);
    int (*write)(uint64_t lba, uint8_t *buf);
} block_dev_t;

extern block_dev_t ata_dev;
extern block_dev_t nvme_dev;

#endif