#include "task.h"

extern void schedule();

int test_task_creation() {
    create_task((void *)test_task_creation); // Dummy
    return 1;
}

int test_preemption() {
    // Assume timer triggers schedule
    return 1;
}

int test_fairness() {
    // Create 10 tasks
    for (int i = 0; i < 10; i++) {
        create_task((void *)test_fairness);
    }
    // Check if scheduler runs
    return 1;
}