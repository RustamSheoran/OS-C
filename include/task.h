#ifndef TASK_H
#define TASK_H

#include <stdint.h>

struct task {
    uint64_t rsp;
    uint64_t rip;
    uint64_t cr3;
    int state;
    struct task *next;
    char stack[4096];
};

void init_scheduler();
void schedule();
void create_task(void (*func)());
void switch_to_task(struct task *next);

#endif