.code16
.global ap_trampoline
ap_trampoline:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    // Enable A20
    in al, 0x92
    or al, 2
    out 0x92, al
    // Load GDT
    lgdt [gdt_ptr - ap_trampoline + 0x8000]
    // Protected mode
    mov eax, cr0
    or al, 1
    mov cr0, eax
    jmp 0x08:protected

.code32
protected:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    // PAE
    mov eax, cr4
    or eax, 0x20
    mov cr4, eax
    // Temp paging
    mov edi, 0x9000
    xor eax, eax
    mov ecx, 4096
    rep stosd
    mov edi, 0x9000
    mov dword [edi], 0x91007
    mov dword [edi + 8], 0x91007
    add edi, 0x1000
    mov dword [edi], 0x92007
    mov dword [edi + 8], 0x92007
    add edi, 0x1000
    mov dword [edi], 0x000083
    mov dword [edi + 8], 0x000083
    mov cr3, edi
    // Long mode
    mov ecx, 0xC0000080
    rdmsr
    or eax, 0x100
    wrmsr
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    jmp 0x08:long_mode

.code64
long_mode:
    mov rax, ap_entry
    jmp rax

gdt:
    .quad 0
    .quad 0x00CF9A000000FFFF
    .quad 0x00CF92000000FFFF
gdt_ptr:
    .word . - gdt - 1
    .quad gdt