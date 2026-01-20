#ifndef IO_H
#define IO_H

#include <stdint.h>

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t val;
    __asm__ volatile ("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

static inline uint64_t rdmsr(uint32_t msr) {
    uint32_t low, high;
    __asm__ volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((uint64_t)high << 32) | low;
}

static inline void wrmsr(uint32_t msr, uint64_t val) {
    __asm__ volatile ("wrmsr" : : "c"(msr), "a"(val & 0xFFFFFFFF), "d"(val >> 32));
}

char serial_getc() {
    while ((inb(0x3F8 + 5) & 1) == 0);
    return inb(0x3F8);
}

void serial_puts(const char *s) {
    while (*s) serial_putc(*s++);
}

char keyboard_getc() {
    while ((inb(0x64) & 1) == 0);
    return inb(0x60);
}

#endif