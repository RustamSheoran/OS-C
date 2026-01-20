#include <stdint.h>
#include "io.h"

struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_middle;
    uint32_t base_high;
    uint32_t reserved;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

struct gdt_entry gdt[6];
struct gdt_ptr gp;

extern void gdt_flush(struct gdt_ptr *ptr);

void init_gdt() {
    gp.limit = sizeof(gdt) - 1;
    gp.base = (uint64_t)&gdt;

    // Null
    gdt[0] = (struct gdt_entry){0};

    // Kernel code
    gdt[1].limit_low = 0xFFFF;
    gdt[1].base_low = 0;
    gdt[1].access = 0x9A;
    gdt[1].granularity = 0xAF;
    gdt[1].base_middle = 0;
    gdt[1].base_high = 0;
    gdt[1].reserved = 0;

    // Kernel data
    gdt[2].limit_low = 0xFFFF;
    gdt[2].base_low = 0;
    gdt[2].access = 0x92;
    gdt[2].granularity = 0xAF;
    gdt[2].base_middle = 0;
    gdt[2].base_high = 0;
    gdt[2].reserved = 0;

    // User code
    gdt[3].limit_low = 0xFFFF;
    gdt[3].base_low = 0;
    gdt[3].access = 0xFA;
    gdt[3].granularity = 0xAF;
    gdt[3].base_middle = 0;
    gdt[3].base_high = 0;
    gdt[3].reserved = 0;

    // User data
    gdt[4].limit_low = 0xFFFF;
    gdt[4].base_low = 0;
    gdt[4].access = 0xF2;
    gdt[4].granularity = 0xAF;
    gdt[4].base_middle = 0;
    gdt[4].base_high = 0;
    gdt[4].reserved = 0;

    // TSS stub
    gdt[5] = (struct gdt_entry){0};
    gdt[5].access = 0x89;

    gdt_flush(&gp);
}