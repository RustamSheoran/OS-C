#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

struct process {
    uint64_t pid;
    uint64_t cr3;
    uint64_t rsp_user;
    uint64_t rip;
    uint64_t stack_top;
    struct process *next;
};

void init_processes();
struct process *create_process(void *elf_data);
void switch_to_process(struct process *proc);

#endif