#include <stdint.h>
#include "io.h"
#include "task.h"

uint64_t syscall_dispatch(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    (void)arg3;
    switch (syscall_num) {
        case 0:
            if (arg1 == 0 && arg2 != 0) {
                char c;
                while ((inb(0x3F8 + 5) & 1) == 0);
                c = inb(0x3F8);
                *(char *)arg2 = c;
                return 1;
            }
            break;
            return (uint64_t)-1;
        case 1:
            if (arg1 == 1) {
                serial_putc((char)arg2);
                return 1;
            }
            break;
            return (uint64_t)-1;
        case 2:
            schedule();
            return 0;
    }
    return (uint64_t)-1;
}
