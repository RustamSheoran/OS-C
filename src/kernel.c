#include <stddef.h>
#include <stdint.h>
#include "fs.h"
#include "graphics.h"
#include "interrupts.h"
#include "io.h"
#include "kernel.h"
#include "paging.h"
#include "pmm.h"
#include "task.h"

#define HEAP_BASE 0x02000000ULL
#define HEAP_SIZE 0x00800000ULL

uint64_t ticks = 0;

struct heap_block {
    uint64_t size;
    uint64_t used;
    struct heap_block *next;
};

static struct heap_block *heap_head = NULL;

void init_serial(void) {
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x80);
    outb(0x3F8 + 0, 0x03);
    outb(0x3F8 + 1, 0x00);
    outb(0x3F8 + 3, 0x03);
    outb(0x3F8 + 2, 0xC7);
    outb(0x3F8 + 4, 0x0B);
}

void serial_putc(char c) {
    while ((inb(0x3F8 + 5) & 0x20) == 0) { }
    outb(0x3F8, (uint8_t)c);
}

void *memset(void *dst, int c, size_t n) {
    uint8_t *p = (uint8_t *)dst;
    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }
    return dst;
}

void *memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dst;
}

size_t strlen(const char *s) {
    size_t n = 0;
    while (s[n]) {
        n++;
    }
    return n;
}

int strcmp(const char *a, const char *b) {
    while (*a && *b && *a == *b) {
        a++;
        b++;
    }
    return (int)((unsigned char)*a - (unsigned char)*b);
}

int strncmp(const char *a, const char *b, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (a[i] != b[i]) {
            return (int)((unsigned char)a[i] - (unsigned char)b[i]);
        }
        if (a[i] == '\0') {
            return 0;
        }
    }
    return 0;
}

void *kmalloc(uint64_t size) {
    if (heap_head == NULL) {
        heap_head = (struct heap_block *)(uintptr_t)HEAP_BASE;
        heap_head->size = HEAP_SIZE - sizeof(struct heap_block);
        heap_head->used = 0;
        heap_head->next = NULL;
    }

    if (size == 0) {
        return NULL;
    }

    size = (size + 15ULL) & ~15ULL;

    struct heap_block *block = heap_head;
    while (block != NULL) {
        if (!block->used && block->size >= size) {
            uint64_t remaining = block->size - size;
            if (remaining > sizeof(struct heap_block) + 16ULL) {
                struct heap_block *next =
                    (struct heap_block *)((uint8_t *)(block + 1) + size);
                next->size = remaining - sizeof(struct heap_block);
                next->used = 0;
                next->next = block->next;
                block->next = next;
                block->size = size;
            }

            block->used = 1;
            return (void *)(block + 1);
        }
        block = block->next;
    }

    return NULL;
}

void kfree(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    struct heap_block *block = ((struct heap_block *)ptr) - 1;
    block->used = 0;

    struct heap_block *cur = heap_head;
    while (cur != NULL && cur->next != NULL) {
        if (!cur->used && !cur->next->used) {
            uint8_t *cur_end = (uint8_t *)(cur + 1) + cur->size;
            if (cur_end == (uint8_t *)cur->next) {
                cur->size += sizeof(struct heap_block) + cur->next->size;
                cur->next = cur->next->next;
                continue;
            }
        }
        cur = cur->next;
    }
}

void panic(const char *msg) {
    serial_puts("PANIC: ");
    serial_puts(msg);
    serial_putc('\n');
    __asm__ volatile ("cli");
    while (1) {
        __asm__ volatile ("hlt");
    }
}

void print_num(uint64_t n) {
    char buf[32];
    int i = 0;

    if (n == 0) {
        serial_putc('0');
        return;
    }

    while (n != 0) {
        buf[i++] = (char)('0' + (n % 10ULL));
        n /= 10ULL;
    }
    while (i > 0) {
        serial_putc(buf[--i]);
    }
}

void ap_entry(void) {
    while (1) {
        __asm__ volatile ("hlt");
    }
}

static struct inode *find_inode(const char *name) {
    for (int i = 0; i < root_fs.num_inodes; i++) {
        if (strcmp(root_fs.inodes[i].name, name) == 0) {
            return &root_fs.inodes[i];
        }
    }
    return NULL;
}

static void shell_help(void) {
    serial_puts("Commands: ls, echo, cat, uptime, meminfo, draw, help\n");
}

static void shell_run(const char *cmd) {
    if (strcmp(cmd, "") == 0) {
        return;
    }

    if (strcmp(cmd, "ls") == 0) {
        for (int i = 0; i < root_fs.num_inodes; i++) {
            serial_puts(root_fs.inodes[i].name);
            serial_putc('\n');
        }
        return;
    }

    if (strncmp(cmd, "echo", 4) == 0) {
        const char *msg = cmd + 4;
        if (*msg == ' ') {
            msg++;
        }
        serial_puts(msg);
        serial_putc('\n');
        return;
    }

    if (strncmp(cmd, "cat", 3) == 0) {
        const char *name = cmd + 3;
        while (*name == ' ') {
            name++;
        }
        if (*name == '\0') {
            serial_puts("usage: cat <file>\n");
            return;
        }

        struct inode *inode = find_inode(name);
        if (inode == NULL) {
            serial_puts("file not found\n");
            return;
        }

        if (inode->size > 0) {
            char *tmp = (char *)kmalloc(inode->size + 1);
            if (tmp == NULL) {
                serial_puts("out of memory\n");
                return;
            }
            if (fs_read_file(name, tmp, inode->size) == 0) {
                tmp[inode->size] = '\0';
                serial_puts(tmp);
                serial_putc('\n');
            } else {
                serial_puts("read failed\n");
            }
            kfree(tmp);
        }
        return;
    }

    if (strcmp(cmd, "uptime") == 0) {
        serial_puts("Uptime: ");
        print_num(ticks / 1000ULL);
        serial_puts(" seconds\n");
        return;
    }

    if (strcmp(cmd, "meminfo") == 0) {
        uint64_t free_pages = pmm_get_free_pages();
        serial_puts("Free pages: ");
        print_num(free_pages);
        serial_puts(" (");
        print_num((free_pages * 4096ULL) / 1024ULL);
        serial_puts(" KiB)\n");
        return;
    }

    if (strcmp(cmd, "draw") == 0) {
        draw_pixel(10, 10, 0x00FF0000U);
        serial_puts("draw ok\n");
        return;
    }

    if (strcmp(cmd, "help") == 0) {
        shell_help();
        return;
    }

    if (strcmp(cmd, "test") == 0) {
        if (run_tests() == 0) {
            serial_puts("tests: PASS\n");
        } else {
            serial_puts("tests: FAIL\n");
        }
        return;
    }

    serial_puts("Unknown command\n");
}

void kernel_main(
    EFI_MEMORY_DESCRIPTOR *MemoryMap,
    UINTN MapSize,
    UINTN DescriptorSize,
    uint64_t kernel_base
) {
    init_serial();
    serial_puts("Kernel loaded\n");

    pmm_init(MemoryMap, MapSize, DescriptorSize);
    paging_init(kernel_base);
    init_gdt();
    init_scheduler();
    init_processes();
    fs_init();
    vfs_init();
    graphics_init();
    init_interrupts();

    fs_create_file("test.txt", 6);
    fs_write_file("test.txt", "hello\n", 6);

    shell_help();
    serial_puts("> ");

    char cmd[128];
    size_t idx = 0;

    while (1) {
        char c = serial_getc();

        if (c == '\r' || c == '\n') {
            serial_putc('\n');
            cmd[idx] = '\0';
            shell_run(cmd);
            idx = 0;
            serial_puts("> ");
            continue;
        }

        if ((c == '\b' || c == 127) && idx > 0) {
            idx--;
            serial_puts("\b \b");
            continue;
        }

        if (idx + 1 < sizeof(cmd)) {
            cmd[idx++] = c;
            serial_putc(c);
        }
    }
}
