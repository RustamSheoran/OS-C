#include <stdint.h>
#include "kernel.h"
#include "task.h"

#define TASK_RUNNABLE 1
#define TASK_CANARY 0xC0DEFACECAFEBEEFULL

struct task *current_task = 0;
struct task *ready_queue = 0;

static void task_init_common(struct task *task) {
    task->state = TASK_RUNNABLE;
    task->canary = TASK_CANARY;
    task->next = task;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(task->cr3));
    __asm__ volatile ("mov %%rsp, %0" : "=r"(task->rsp));
}

void init_scheduler(void) {
    struct task *bootstrap = (struct task *)kmalloc(sizeof(struct task));
    if (bootstrap == 0) {
        panic("scheduler bootstrap allocation failed");
    }

    task_init_common(bootstrap);
    bootstrap->rip = 0;
    current_task = bootstrap;
    ready_queue = bootstrap;
}

void create_task(void (*func)(void)) {
    if (ready_queue == 0 || func == 0) {
        return;
    }

    struct task *task = (struct task *)kmalloc(sizeof(struct task));
    if (task == 0) {
        return;
    }

    task_init_common(task);
    task->rip = (uint64_t)func;

    task->next = ready_queue->next;
    ready_queue->next = task;
}

void schedule(void) {
    if (current_task == 0 || current_task->next == 0) {
        return;
    }
    if (current_task->next == current_task) {
        return;
    }

    switch_to_task(current_task->next);
}

void switch_to_task(struct task *next) {
    if (next == 0 || next == current_task || next->state != TASK_RUNNABLE) {
        return;
    }
    current_task = next;
}
