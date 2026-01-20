#include <stdint.h>
#include "task.h"
#include "pmm.h"
#include "kernel.h"

struct task *current_task;
struct task *ready_queue;

void init_scheduler() {
    current_task = kmalloc(sizeof(struct task));
    current_task->state = 1;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(current_task->cr3));
    ready_queue = current_task;
}

void create_task(void (*func)()) {
    struct task *new_task = kmalloc(sizeof(struct task));
    new_task->rsp = (uint64_t)&new_task->stack[4096];
    new_task->rsp -= 8;
    *(uint64_t *)new_task->rsp = (uint64_t)func;
    new_task->rip = (uint64_t)func;
    new_task->cr3 = current_task->cr3;
    new_task->state = 1;
    new_task->next = ready_queue->next;
    ready_queue->next = new_task;
}

void schedule() {
    if (!ready_queue->next) return;
    struct task *next = ready_queue->next;
    ready_queue = next;
    switch_to_task(next);
}

void switch_to_task(struct task *next) {
    uint64_t old_rsp;
    __asm__ volatile ("mov %%rsp, %0" : "=r"(old_rsp));
    current_task->rsp = old_rsp;
    current_task = next;
    __asm__ volatile ("mov %0, %%rsp" : : "r"(next->rsp));
}