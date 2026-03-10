#include "kernel.h"

void smp_init(void) {
    /*
     * Keep AP bring-up disabled until trampoline/CPU discovery is complete.
     * This keeps single-core boot deterministic under QEMU/OVMF.
     */
}
