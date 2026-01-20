#ifndef NVME_H
#define NVME_H

#include <stdint.h>

void nvme_init();
void nvme_read_sector(uint64_t lba, uint8_t *buf);
void nvme_write_sector(uint64_t lba, uint8_t *buf);

#endif