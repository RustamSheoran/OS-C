#ifndef NVME_H
#define NVME_H

#include <stdint.h>

void nvme_init();
int nvme_read_sector(uint64_t lba, uint8_t *buf);
int nvme_write_sector(uint64_t lba, uint8_t *buf);

#endif
