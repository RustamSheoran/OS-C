#ifndef ATA_H
#define ATA_H

#include <stdint.h>

int ata_read_sector(uint64_t lba, uint8_t *buf);
int ata_write_sector(uint64_t lba, uint8_t *buf);
void ata_init();

#endif