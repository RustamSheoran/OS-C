#include <stdint.h>
#include "nvme.h"
#include "paging.h"

#define NVME_BAR 0xFE000000ULL

void nvme_init() {
    // Map NVMe registers
    for (uint64_t addr = NVME_BAR; addr < NVME_BAR + 0x1000; addr += 0x1000) {
        map_page(addr, addr, 3); // Identity map
    }
    // Initialize controller: reset, set queues, etc.
    // Stub
}

void nvme_read_sector(uint64_t lba, uint8_t *buf) {
    // Submit read command to submission queue
    // Wait for completion
    // Copy data
    // Stub
}

void nvme_write_sector(uint64_t lba, uint8_t *buf) {
    // Submit write command
    // Stub
}