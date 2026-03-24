# OS-C (x86_64 Freestanding UEFI Kernel)

## Overview

OS-C is a freestanding 64-bit operating system kernel written in ISO C11 and x86_64 assembly, designed to explore and understand the boundary between firmware, hardware, and software abstraction.

Unlike typical OS hobby projects, this kernel intentionally avoids libc and external runtime dependencies. Every runtime primitive — memory management, interrupt handling, and execution flow — is implemented directly within the kernel.

The system boots as a UEFI application (`kernel.efi`), transitions control from firmware to kernel-owned memory management, initializes core CPU structures, and enters an interactive shell environment over serial I/O.

---

## Motivation

Modern software development often abstracts away the underlying machine. This project was built to reverse that abstraction — to understand:

* How firmware transfers control to an OS
* How memory is discovered, mapped, and managed
* How interrupts and syscalls form the foundation of execution
* What is required to move from raw hardware to a usable system

The goal is not just to “build an OS”, but to **understand the constraints, tradeoffs, and failure modes of real systems**.

---

## System Architecture

The kernel follows a **monolithic design** with explicit control over all subsystems.

### Boot Flow

1. UEFI loads `kernel.efi`
2. Kernel retrieves memory map
3. Exits boot services safely
4. Transfers control to `kernel_main`
5. Initializes subsystems in deterministic order
6. Enters interactive shell loop

---

### Memory Model

* **Physical Memory Manager (PMM):**

  * Bitmap-based allocator
  * Initialized from EFI memory map
  * Page size: 4 KiB

* **Virtual Memory:**

  * 4-level paging (PML4)
  * Identity mapping (low memory)
  * Higher-half kernel mapping:

    * `0xFFFFFFFF80000000`

* **Heap Allocator:**

  * Bump allocator (8 MiB region)
  * 16-byte aligned
  * No free (intentional simplification)

---

### CPU Initialization

* **GDT:** Kernel/user segments initialized and loaded
* **IDT:** 256-entry table with interrupt gates
* **Interrupts:**

  * PIC remapped (0x20–0x2F)
  * PIT configured at 1 kHz

---

### Syscall Interface

* Uses `syscall/sysret` instructions
* MSRs configured:

  * `STAR`, `LSTAR`, `FMASK`, `EFER`
* Assembly stubs bridge user/kernel boundary

---

### Scheduling Model

* Round-robin task list
* Circular linked structure
* Current limitation:

  * No full register context switching yet

---

### Filesystem & Shell

* In-memory filesystem (flat inode structure)
* Basic VFS abstraction layer
* Interactive shell commands:

```
ls
echo <text>
cat <file>
uptime
meminfo
draw
help
```

---

## Key Design Decisions

### Why bitmap allocator?

Chosen for simplicity and deterministic behavior during early boot. While less efficient than buddy allocation, it provides predictable allocation patterns and low implementation complexity.

### Why higher-half kernel?

Separates kernel space from user space and mirrors real OS design patterns, improving safety and clarity in address space layout.

### Why avoid libc?

To maintain full control over runtime behavior and avoid hidden abstractions that obscure low-level mechanics.

### Why UEFI instead of BIOS?

UEFI provides structured interfaces (memory map, protocols) and is closer to modern system initialization flows.

---

## Technical Challenges

### 1. ExitBootServices instability

UEFI requires the memory map to remain unchanged between retrieval and exit. Early implementations failed due to mismatched map keys, requiring a retry-safe loop.

---

### 2. Paging-induced triple faults

Incorrect page table setup caused CPU resets. Debugging required careful validation of identity mapping and CR3 loading.

---

### 3. Interrupt stack corruption

Improper ISR setup initially corrupted execution state. Fixed by enforcing strict register save/restore discipline in assembly stubs.

---

### 4. Kernel memory ownership transition

Ensuring safe transition from firmware-managed memory to kernel-managed memory required careful sequencing of PMM initialization.

---

## Repository Structure

* `src/efi_main.c` — UEFI entry and boot transition
* `src/kernel.c` — Kernel initialization and main loop
* `src/pmm.c` — Physical memory allocator
* `src/paging.c` — Virtual memory setup
* `src/gdt.c`, `src/start.S` — GDT initialization
* `src/idt.c`, `src/interrupt_stubs.S` — Interrupt system
* `src/scheduler.c` — Task management
* `src/fs.c`, `src/vfs.c` — Filesystem layer
* `src/graphics.c` — Framebuffer abstraction
* `scripts/linker.ld` — Higher-half linker script

---

## Build & Execution

### Requirements

* x86_64-elf GCC toolchain
* QEMU
* OVMF firmware

### Build

```
make
```

### Run

```
./scripts/run.sh
```

---

## Validation

Inside the kernel shell:

```
ls
echo hello
cat test.txt
uptime
meminfo
draw
help
```

---

## Known Limitations

* No page fault handler
* No full context switching
* No complete user-mode execution path
* Limited driver implementations

These are intentional next steps toward a more complete system.

---

## Future Work

* Demand paging and page fault handling
* Full context switching (register + stack state)
* User-mode process execution
* SMP (multi-core support)
* Persistent filesystem

---

## Conclusion

OS-C is an exploration of system fundamentals — from firmware interfaces to kernel execution — with a focus on understanding rather than abstraction.

This project represents an ongoing effort to move from “writing programs” to **engineering systems**.
