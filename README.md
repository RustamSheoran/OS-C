# OS-C (x86_64 Freestanding UEFI Kernel)

OS-C is a freestanding x86_64 kernel written in ISO C11 and x86_64 assembly.
It boots as a UEFI application (`kernel.efi`), switches to kernel-owned memory
management, initializes core CPU tables (GDT/IDT), configures timer interrupts,
and enters a serial interactive shell.

This repository intentionally avoids libc/glibc and external runtime libraries.
All runtime primitives used by the kernel are implemented in-tree.

## 1. Scope and Current State

Implemented and wired in boot path:
- UEFI entry (`efi_main`) and exit from boot services.
- EFI memory-map handoff to kernel.
- Bitmap physical page allocator.
- 4-level paging setup with identity + higher-half mapping.
- GDT load and IDT load.
- PIC remap + PIT setup + timer interrupt counting.
- `syscall` entry and C dispatcher.
- Minimal round-robin task list metadata.
- In-memory filesystem and shell command dispatcher.
- Minimal graphics buffer and pixel write primitive.
- ATA/NVMe/RTL8139 compile-safe minimal implementations.

Current shell commands:
- `ls`
- `echo <text>`
- `cat <file>`
- `uptime`
- `meminfo`
- `draw`
- `help`

## 2. Repository Layout

- `src/efi_main.c`: UEFI ABI entrypoint and boot-service exit logic.
- `src/kernel.c`: early serial init, subsystem init order, shell loop.
- `src/pmm.c`: bitmap physical memory allocator.
- `src/paging.c`: PML4/PDPT/PD/PT mapping functions.
- `src/gdt.c` + `src/start.S`: GDT descriptor setup and `lgdt`/segment reload.
- `src/idt.c` + `src/interrupt_stubs.S`: IDT and ISR/syscall assembly stubs.
- `src/interrupts.c`: PIC/PIT/syscall MSR configuration.
- `src/scheduler.c`: task structures and simple scheduler transitions.
- `src/fs.c`, `src/vfs.c`: in-memory FS and VFS shim.
- `src/graphics.c`: software framebuffer backing store.
- `src/ata.c`, `src/nvme.c`, `src/net.c`: device layer stubs/minimal logic.
- `scripts/linker.ld`: higher-half linker script.
- `scripts/run.sh`: UEFI boot in QEMU/OVMF over serial stdio.

## 3. Build System (Freestanding)

`Makefile` compiles with:
- `-ffreestanding`
- `-fno-stack-protector`
- `-fno-pie`
- `-mcmodel=kernel`
- `-mno-red-zone`
- `-mgeneral-regs-only`
- `-std=c11 -Wall -Wextra`

Link flags:
- `-T scripts/linker.ld`
- `-nostdlib`

Output artifacts:
- `kernel.elf` (linked kernel image)
- `kernel.efi` (UEFI executable via `objcopy`)

Default toolchain variables:
- `CC=x86_64-elf-gcc`
- `AS=x86_64-elf-as`
- `LD=x86_64-elf-ld`
- `OBJCOPY=x86_64-elf-objcopy`

## 4. Toolchain and Runtime Prerequisites

Required for target boot path:
- x86_64-elf cross GCC + binutils
- QEMU (`qemu-system-x86_64`)
- OVMF firmware (`OVMF.fd` / `OVMF_CODE.fd`)

Expected OVMF search paths in `scripts/run.sh`:
- `/usr/share/ovmf/OVMF.fd`
- `/usr/share/OVMF/OVMF.fd`
- `/usr/share/OVMF/OVMF_CODE.fd`
- `/usr/share/edk2/ovmf/OVMF.fd`

## 5. Build and Run

Build:

```bash
make
```

Run:

```bash
./scripts/run.sh
```

Run script behavior:
1. Builds kernel.
2. Creates EFI System Partition tree: `esp/efi/boot/bootx64.efi`.
3. Boots QEMU with OVMF firmware and serial redirected to stdio.

## 6. Boot Flow (Detailed)

### 6.1 UEFI entry

`efi_main(ImageHandle, SystemTable)`:
1. Uses `HandleProtocol` with `EFI_LOADED_IMAGE_PROTOCOL_GUID` to capture image base.
2. Calls `GetMemoryMap` with null buffer to get required size.
3. Allocates pages for memory map buffer (`AllocatePages`).
4. Re-fetches memory map.
5. Calls `ExitBootServices` in retry-safe loop.
6. Transfers control to:

```c
kernel_main(MemoryMap, MapSize, DescriptorSize, kernel_base)
```

### 6.2 Kernel init order

`kernel_main` executes subsystems in this order:
1. `init_serial()`
2. `pmm_init(MemoryMap, MapSize, DescriptorSize)`
3. `paging_init(kernel_base)`
4. `init_gdt()`
5. `init_scheduler()`
6. `init_processes()`
7. `fs_init()`
8. `vfs_init()`
9. `graphics_init()`
10. `init_interrupts()`
11. Seeds `test.txt` in root FS
12. Enters serial shell loop

## 7. Memory Management

### 7.1 Physical memory allocator (`src/pmm.c`)

- Bitmap base physical address: `0x00100000` (1 MiB).
- Page size: `4096`.
- Initializes all pages as used, then clears bits for EFI `EfiConventionalMemory`.
- Reserves bitmap storage pages.
- Reserves page 0.
- Allocation:
  - first-fit scan from page index 1 upward.
  - returns physical address `page_index * 4096`.
- Free:
  - bounds checks + idempotence checks.
- Runtime metric:
  - `pmm_get_free_pages()`.

### 7.2 Heap allocator (`kmalloc`)

- Bump allocator range:
  - base: `0x02000000`
  - size: `0x00800000` (8 MiB)
- 16-byte alignment.
- Returns `NULL` on exhaustion.
- `kfree` is currently no-op by design.

### 7.3 Paging (`src/paging.c`)

- Allocates PML4 via PMM.
- Identity maps low 1 GiB (`0x00000000`..`0x3FFFFFFF`) as RW.
- Maps higher-half kernel virtual window:
  - virtual base `0xFFFFFFFF80000000`
  - mapped size `0x04000000` (64 MiB)
  - physical source base is UEFI image base (`kernel_base`).
- Loads CR3 to activate page tables.

## 8. CPU Tables and Interrupts

### 8.1 GDT

`src/gdt.c` defines:
- null descriptor
- kernel code/data segments
- user code/data segments
- reserved TSS slot

`src/start.S:gdt_flush`:
- `lgdt`
- reloads segment registers
- far return to reload CS

### 8.2 IDT

`src/idt.c`:
- zero-initializes 256 entries
- installs timer gate at vector `0x20`
- executes `lidt`

### 8.3 PIC/PIT/syscall

`src/interrupts.c`:
- PIC remap to `0x20`..`0x2F`
- masks all IRQs except PIT IRQ0
- PIT configured to 1 kHz
- syscall MSRs:
  - STAR (`0xC0000081`)
  - LSTAR (`0xC0000082`)
  - FMASK (`0xC0000084`)
  - EFER.SCE (`0xC0000080`)

`src/interrupt_stubs.S`:
- `isr_timer`: saves registers, calls `timer_tick`, sends EOI, `iretq`.
- `syscall_handler`: marshals args into `syscall_dispatch`, returns with `sysretq`.

## 9. Scheduler and Processes

### 9.1 Scheduler

`src/scheduler.c` provides:
- bootstrap task initialization
- singly-linked circular runnable list
- task creation (`create_task`)
- next-task selection (`schedule`)

Current context-switch semantics are minimal:
- updates `current_task` pointer and validates state
- full register context switching is not yet implemented

### 9.2 Process model

`src/process.c` provides:
- process list and PID management
- ELF load callout
- user stack page mapping
- CR3 switch in `switch_to_process`

User-mode transition (`iretq`/`sysret` process entry) is not yet complete.

## 10. Filesystem and Shell Behavior

### 10.1 FS (`src/fs.c`)

- Flat inode array (`MAX_FILES`).
- Max file payload currently one page (4 KiB cap).
- `fs_create_file`, `fs_write_file`, `fs_read_file`.

### 10.2 VFS (`src/vfs.c`)

- In-memory vnode table (`MAX_VNODES`).
- Name-based vnode lookup.
- `ramfs` read/write handlers.
- NTFS interface scaffolding and NVMe probe path retained for extension.

### 10.3 Shell command semantics

- `ls`: prints all inode names in root fs.
- `echo <text>`: prints exact suffix after `echo`.
- `cat <file>`: reads and prints file contents, reports missing file.
- `uptime`: prints `ticks / 1000`.
- `meminfo`: prints free pages and KiB.
- `draw`: writes red pixel to software framebuffer.
- `help`: prints command list.

## 11. Device Layer

- `src/graphics.c`: software framebuffer backing array (`320x200`).
- `src/ata.c`: ATA PIO sector read/write paths.
- `src/net.c`: non-crashing RTL8139 init + loopback send/recv buffer.
- `src/nvme.c`: compile-safe NVMe command skeleton.
- `src/smp.c`: deterministic no-op AP bring-up placeholder to avoid unstable boot.

## 12. Validation Procedure

Recommended validation on a properly provisioned host:

1. `make clean && make`
2. verify artifacts:
   - `file kernel.elf`
   - `file kernel.efi`
3. `./scripts/run.sh`
4. in serial shell, run:
   - `ls`
   - `echo hello`
   - `cat test.txt`
   - `uptime`
   - `meminfo`
   - `draw`
   - `help`

## 13. Debug Notes

- Panic path: `panic()` prints and halts with `cli` + `hlt`.
- Serial port:
  - COM1 base `0x3F8`
  - polled TX/RX mode
- If boot hangs before serial output:
  - verify OVMF file exists and is readable
  - verify `kernel.efi` copied to `esp/efi/boot/bootx64.efi`
  - verify cross-toolchain binaries are first in `PATH`

## 14. Known Limitations

- No demand paging/page-fault handler yet.
- No full register context switch for multitasking.
- No completed user-mode launch path.
- No full SMP/AP startup implementation.
- Driver layers are intentionally minimal and not production complete.

## 15. License / Contribution

Project contribution is code-review driven. Keep all changes freestanding and
compatible with x86_64 UEFI boot assumptions used by this tree.
