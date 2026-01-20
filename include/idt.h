#ifndef IDT_H
#define IDT_H

#include <stdint.h>

struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
};

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
};

void init_idt();
void set_idt_gate(int n, uint64_t handler);

#endif