#include <stdint.h>
#include "process.h"
#include "pmm.h"
#include "paging.h"
#include "elf_loader.h"
#include "kernel.h"

struct process *current_process;
struct process *process_list;
uint64_t next_pid = 1;

void init_processes() {
    current_process = NULL;
    process_list = NULL;
}

struct process *create_process(void *elf_data) {
    struct process *proc = kmalloc(sizeof(struct process));
    if (!proc) return NULL;
    proc->pid = next_pid++;
    // Share kernel page table for now
    __asm__ volatile ("mov %%cr3, %0" : "=r"(proc->cr3));
    uint64_t entry;
    if (load_elf(elf_data, &entry) != 0) {
        // kfree(proc);
        return NULL;
    }
    proc->rip = entry;
    proc->stack_top = 0x7FFFFFFFF000;
    // Alloc user stack
    for (uint64_t addr = 0x7FFFFFFFF000 - 0x1000; addr >= 0x7FFFFFFFE000; addr -= 0x1000) {
        uint64_t phys = pmm_alloc_page();
        map_page(addr, phys, 7); // User RW
    }
    proc->rsp_user = 0x7FFFFFFFF000;
    proc->next = process_list;
    process_list = proc;
    return proc;
}

void switch_to_process(struct process *proc) {
    __asm__ volatile ("mov %0, %%cr3" : : "r"(proc->cr3));
    // TODO: Enter user mode with iretq
}