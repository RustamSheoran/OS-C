#include <stdint.h>
#include <stddef.h>
#include "io.h"
#include "kernel.h"
#include "pmm.h"
#include "paging.h"
#include "interrupts.h"
#include "task.h"
#include "fs.h"

void init_serial() {
    outb(0x3F8 + 1, 0x00); // Disable interrupts
    outb(0x3F8 + 3, 0x80); // Enable DLAB
    outb(0x3F8 + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    outb(0x3F8 + 1, 0x00); // (hi byte)
    outb(0x3F8 + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(0x3F8 + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    outb(0x3F8 + 4, 0x0B); // RTS/DSR set
}

void serial_putc(char c) {
    while ((inb(0x3F8 + 5) & 0x20) == 0);
    outb(0x3F8, c);
}

static uint64_t heap_ptr = 0xFFFFFFFF80100000;

void *kmalloc(size_t size) {
    void *ptr = (void *)heap_ptr;
    heap_ptr += size;
    return ptr;
}

void kfree(void *ptr) {
    // Simple bump, no free
}

int strcmp(const char *a, const char *b) {
    while (*a && *b && *a == *b) { a++; b++; }
    return *a - *b;
}

uint64_t ticks = 0;

void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *d = dest;
    const uint8_t *s = src;
    for (size_t i = 0; i < n; i++) *d++ = *s++;
    return dest;
}

void *memset(void *s, int c, size_t n) {
    uint8_t *p = s;
    for (size_t i = 0; i < n; i++) *p++ = c;
    return s;
}

void panic(const char *msg) {
    serial_puts("PANIC: ");
    serial_puts(msg);
    serial_putc('\n');
    while (1);
}

void print_num(uint64_t n) {
    if (n == 0) {
        serial_putc('0');
        return;
    }
    char buf[20];
    int i = 0;
    while (n) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }
    while (i--) serial_putc(buf[i]);
}

void ap_entry() {
    serial_puts("AP online\n");
    while (1);
}

void task_func() {
    while (1) {
        serial_putc('T');
    }
}

void kernel_main(EFI_MEMORY_DESCRIPTOR *MemoryMap, UINTN MapSize, UINTN DescriptorSize, uint64_t kernel_base) {
    init_serial();
    const char *msg = "Kernel loaded\n";
    for (const char *p = msg; *p; p++) {
        serial_putc(*p);
    }
    pmm_init(MemoryMap, MapSize, DescriptorSize);
    paging_init(kernel_base);
    init_gdt();
    init_interrupts();
    smp_init();
    init_scheduler();
    init_processes();
    create_task(task_func);
    fs_init();
    vfs_init();
    fs_create_file("test.txt", 10);
    fs_write_file("test.txt", "hello", 5);
    // Test alloc/free
    uint64_t page = pmm_alloc_page();
    if (page) {
        serial_putc('A');
        pmm_free_page(page);
        serial_putc('F');
    }
    // Test kmalloc
    void *ptr = kmalloc(100);
    if (ptr) serial_putc('K');
    // Test schedule
    schedule();
    // Test syscall
    __asm__ volatile ("mov $1, %%rax; mov $72, %%rdi; syscall" : : : "rax", "rdi");

    // Shell
    serial_puts("> ");
    char cmd[100];
    int idx = 0;
    while (1) {
        char c = serial_getc();
        if (c == '\r' || c == '\n') {
            cmd[idx] = 0;
        if (strcmp(cmd, "ls") == 0) {
            for (int i = 0; i < root_fs.num_inodes; i++) {
                serial_puts(root_fs.inodes[i].name);
                serial_putc('\n');
            }
        } else if (strcmp(cmd, "echo hello") == 0) {
            serial_puts("hello\n");
        } else if (strcmp(cmd, "cat test.txt") == 0) {
            char buf[10];
            fs_read_file("test.txt", buf, 5);
            buf[5] = 0;
            serial_puts(buf);
            serial_putc('\n');
        } else if (strcmp(cmd, "help") == 0) {
            serial_puts("Commands: ls, echo hello, cat test.txt, uptime, meminfo, help\n");
        } else if (strcmp(cmd, "uptime") == 0) {
            serial_puts("Uptime: ");
            print_num(ticks / 1000);
            serial_puts(" seconds\n");
        } else if (strcmp(cmd, "meminfo") == 0) {
            serial_puts("Free pages: ");
            print_num(pmm_get_free_pages());
            serial_putc('\n');
        } else if (strcmp(cmd, "draw") == 0) {
            draw_pixel(100, 100, 0xFF0000);
            serial_puts("Pixel drawn\n");
        } else if (strcmp(cmd, "test") == 0) {
            run_tests();
        } else if (strcmp(cmd, "") == 0) {
            // do nothing
        } else {
            serial_puts("Unknown command\n");
        }
            serial_puts("> ");
            idx = 0;
        } else {
            cmd[idx++] = c;
            if (idx >= 99) idx = 99;
        }
    }
}

