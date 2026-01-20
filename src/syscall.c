#include <stdint.h>
#include "io.h"
#include "task.h"

void syscall_dispatch(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    (void)arg3; // Suppress unused parameter warning
    switch (syscall_num) {
        case 0: // read
            if (arg1 == 0) { // stdin
                char c;
                while ((inb(0x3F8 + 5) & 1) == 0);
                c = inb(0x3F8);
                *(char *)arg2 = c;
            }
            break;
        case 1: // write
            if (arg1 == 1) { // stdout
                serial_putc((char)arg2);
            }
            break;
        case 2: // yield
            schedule();
            break;
    }
}