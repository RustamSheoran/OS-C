.section .text

.global load_idt
load_idt:
    lidt (%rdi)
    ret

.global syscall_handler
syscall_handler:
    push %rcx
    push %r11
    push %rdi
    push %rsi
    push %rdx

    mov %rax, %rdi
    mov 16(%rsp), %rsi
    mov 8(%rsp), %rdx
    mov 0(%rsp), %rcx
    call syscall_dispatch

    pop %rdx
    pop %rsi
    pop %rdi
    pop %r11
    pop %rcx
    sysretq

.global isr_timer
isr_timer:
    push %rax
    push %rbx
    push %rcx
    push %rdx
    push %rbp
    push %rsi
    push %rdi
    push %r8
    push %r9
    push %r10
    push %r11
    push %r12
    push %r13
    push %r14
    push %r15

    call timer_tick

    mov $0x20, %al
    outb %al, $0x20

    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rdi
    pop %rsi
    pop %rbp
    pop %rdx
    pop %rcx
    pop %rbx
    pop %rax
    iretq
