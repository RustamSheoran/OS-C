#include <stdint.h>

#define LAPIC_VIRT 0xFFFFFFFFFF000000ULL

static inline uint32_t lapic_read(uint32_t reg) {
    return *(volatile uint32_t*)(LAPIC_VIRT + reg);
}

static inline void lapic_write(uint32_t reg, uint32_t val) {
    *(volatile uint32_t*)(LAPIC_VIRT + reg) = val;
}

void smp_init() {
    // Send INIT to AP 1
    lapic_write(0x310, (1 << 24)); // ICR high: dest APIC ID 1
    lapic_write(0x300, (5 << 8) | 5); // INIT, level assert
    // Wait 10ms
    for (volatile int i = 0; i < 1000000; i++);
    lapic_write(0x300, (5 << 8) | 4); // INIT, level deassert
    // Wait
    for (volatile int i = 0; i < 1000000; i++);
    // Send SIPI
    lapic_write(0x310, (1 << 24));
    lapic_write(0x300, (6 << 8) | 0x08); // SIPI, vector 0x80
    // Wait
    for (volatile int i = 0; i < 1000000; i++);
    // Send SIPI again
    lapic_write(0x310, (1 << 24));
    lapic_write(0x300, (6 << 8) | 0x08);
}