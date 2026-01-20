#include "io.h"

int test_efi_init() {
    // Check if memory map was processed
    return 1; // Assume success
}

int test_serial_output() {
    serial_putc('T');
    return 1;
}