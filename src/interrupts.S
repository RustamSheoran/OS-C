.section .text

.global load_idt

load_idt:
    lidt (%rdi)
    ret

.global syscall_handler

syscall_handler:
    push %rbx
    push %rcx
    push %rdx
    push %rbp
    push %r8
    push %r9
    push %r10
    push %r11
    push %r12
    push %r13
    push %r14
    push %r15

    # Args: rax = num, rdi = a1, rsi = a2, rdx = a3

    push %rdi
    push %rsi
    push %rdx

    mov %rax, %rdi
    mov 24(%rsp), %rsi  # saved rdi
    mov 16(%rsp), %rdx  # saved rsi
    mov 8(%rsp), %rcx   # saved rdx

    call syscall_dispatch

    add $24, %rsp  # pop the pushed args

    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rbp
    pop %rdx
    pop %rcx
    pop %rbx

    iretq

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

    call schedule

    # Send EOI
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