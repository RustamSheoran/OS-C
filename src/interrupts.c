#include <stdint.h>
#include "io.h"
#include "interrupts.h"
#include "idt.h"

void init_idt();
void init_pic();
void init_pit();

extern void syscall_handler();

void init_syscall();

void init_interrupts() {
    init_idt();
    init_pic();
    init_pit();
    init_syscall();
    __asm__ volatile ("sti");
}

extern uint64_t ticks;

void timer_tick() {
    ticks++;
    schedule();
}

void init_syscall() {
    wrmsr(0xC0000081, (0x08ULL << 32) | (0x18ULL << 48));
    wrmsr(0xC0000082, (uint64_t)syscall_handler);
    wrmsr(0xC0000084, 0);
    uint64_t efer = rdmsr(0xC0000080);
    efer |= (1ULL << 0);
    wrmsr(0xC0000080, efer);
}

void init_pic() {
    // Remap PIC
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    // Mask all except timer
    outb(0x21, 0xFE);
    outb(0xA1, 0xFF);
}

void init_pit() {
    uint32_t divisor = 1193182 / 1000;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}