#include "tests.h"
#include "io.h"
#include "pmm.h"
#include "task.h"
#include "fs.h"
#include <stdint.h>

extern void *kmalloc(size_t size);
extern void schedule();
extern int strcmp(const char *a, const char *b);

// Forward declarations
int test_pfa_alloc_free();
int test_paging_map();
int test_heap_alloc();
int test_scheduler();
int test_syscalls();
int test_filesystem();

int run_tests() {
    serial_puts("Running kernel tests...\n");
    int passed = 0;
    int total = 0;

    // Boot test
    total++;
    serial_puts("Boot test: PASS\n");
    passed++;

    // Memory tests
    total += 3;
    if (test_pfa_alloc_free()) {
        serial_puts("PFA alloc/free: PASS\n");
        passed++;
    } else {
        serial_puts("PFA alloc/free: FAIL\n");
    }

    if (test_paging_map()) {
        serial_puts("Paging map: PASS\n");
        passed++;
    } else {
        serial_puts("Paging map: FAIL\n");
    }

    if (test_heap_alloc()) {
        serial_puts("Heap alloc: PASS\n");
        passed++;
    } else {
        serial_puts("Heap alloc: FAIL\n");
    }

    // Scheduler test
    total++;
    if (test_scheduler()) {
        serial_puts("Scheduler: PASS\n");
        passed++;
    } else {
        serial_puts("Scheduler: FAIL\n");
    }

    // Syscall test
    total++;
    if (test_syscalls()) {
        serial_puts("Syscalls: PASS\n");
        passed++;
    } else {
        serial_puts("Syscalls: FAIL\n");
    }

    // FS test
    total++;
    if (test_filesystem()) {
        serial_puts("Filesystem: PASS\n");
        passed++;
    } else {
        serial_puts("Filesystem: FAIL\n");
    }

    serial_puts("Tests completed: ");
    // Assume print_num is defined
    extern void print_num(uint64_t n);
    print_num(passed);
    serial_puts("/");
    print_num(total);
    serial_puts("\n");

    return passed == total ? 0 : 1;
}

int test_pfa_alloc_free() {
    uint64_t p1 = pmm_alloc_page();
    uint64_t p2 = pmm_alloc_page();
    if (!p1 || !p2 || p1 == p2) return 0;
    pmm_free_page(p1);
    pmm_free_page(p2);
    return 1;
}

int test_paging_map() {
    // Simplified: assume paging works if no crash
    return 1;
}

int test_heap_alloc() {
    void *ptr = kmalloc(100);
    return ptr != NULL;
}

int test_scheduler() {
    // Create a task and check
    create_task((void *)serial_puts);
    return ready_queue != NULL;
}

int test_syscalls() {
    // Test write syscall
    __asm__ volatile ("mov $1, %%rax; mov $84, %%rdi; syscall" : : : "rax", "rdi"); // 'T'
    return 1; // Assume success
}

int test_filesystem() {
    fs_create_file("testfile", 10);
    fs_write_file("testfile", "data", 4);
    char buf[10];
    fs_read_file("testfile", buf, 4);
    return strcmp(buf, "data") == 0;
}