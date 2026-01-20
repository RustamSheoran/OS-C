#include <stdint.h>
#include "idt.h"
#include "io.h"

struct idt_entry idt[256];
struct idt_ptr idtp;

extern void load_idt(struct idt_ptr *ptr);

extern void isr_timer();

void init_idt() {
    idtp.limit = sizeof(idt) - 1;
    idtp.base = (uint64_t)&idt;

    // Initialize IDT with zeros
    for (int i = 0; i < 256; i++) {
        idt[i].offset_low = 0;
        idt[i].selector = 0;
        idt[i].ist = 0;
        idt[i].type_attr = 0;
        idt[i].offset_mid = 0;
        idt[i].offset_high = 0;
        idt[i].zero = 0;
    }

    // Set timer ISR
    set_idt_gate(0x20, (uint64_t)isr_timer);

    load_idt(&idtp);
}

void set_idt_gate(int n, uint64_t handler) {
    idt[n].offset_low = handler & 0xFFFF;
    idt[n].selector = 0x08; // Kernel code segment
    idt[n].ist = 0;
    idt[n].type_attr = 0x8E; // Present, ring 0, interrupt gate
    idt[n].offset_mid = (handler >> 16) & 0xFFFF;
    idt[n].offset_high = handler >> 32;
    idt[n].zero = 0;
}