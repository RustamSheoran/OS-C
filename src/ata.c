#include <stdint.h>
#include "ata.h"
#include "io.h"

#define ATA_DATA 0x1F0
#define ATA_ERROR 0x1F1
#define ATA_SECTOR_COUNT 0x1F2
#define ATA_LBA_LOW 0x1F3
#define ATA_LBA_MID 0x1F4
#define ATA_LBA_HIGH 0x1F5
#define ATA_DRIVE 0x1F6
#define ATA_STATUS 0x1F7
#define ATA_COMMAND 0x1F7

void ata_wait_bsy() {
    while (inb(ATA_STATUS) & 0x80);
}

void ata_wait_drq() {
    while (!(inb(ATA_STATUS) & 0x08));
}

int ata_read_sector(uint64_t lba, uint8_t *buf) {
    ata_wait_bsy();
    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECTOR_COUNT, 1);
    outb(ATA_LBA_LOW, lba & 0xFF);
    outb(ATA_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_LBA_HIGH, (lba >> 16) & 0xFF);
    outb(ATA_COMMAND, 0x20);
    ata_wait_bsy();
    ata_wait_drq();
    for (int i = 0; i < 256; i++) {
        uint16_t data = inw(ATA_DATA);
        buf[i*2] = data & 0xFF;
        buf[i*2+1] = data >> 8;
    }
    return 0;
}

int ata_write_sector(uint64_t lba, uint8_t *buf) {
    ata_wait_bsy();
    outb(ATA_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECTOR_COUNT, 1);
    outb(ATA_LBA_LOW, lba & 0xFF);
    outb(ATA_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_LBA_HIGH, (lba >> 16) & 0xFF);
    outb(ATA_COMMAND, 0x30);
    ata_wait_bsy();
    ata_wait_drq();
    for (int i = 0; i < 256; i++) {
        uint16_t data = buf[i*2] | (buf[i*2+1] << 8);
        outw(ATA_DATA, data);
    }
    outb(ATA_COMMAND, 0xE7);
    ata_wait_bsy();
    return 0;
}

void ata_init() {
    // Identify if needed
}

#include "block.h"

block_dev_t ata_dev = {
    .read = ata_read_sector,
    .write = ata_write_sector
};