.section .text

.global ap_trampoline
ap_trampoline:
    cli
1:
    hlt
    jmp 1b
