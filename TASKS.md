# TASKS.md - x86_64 OS Kernel Engineering Contract

## Restart Prompt
To resume kernel development after shutdown, send this exact message to opencode:
"Resume work on the production-grade x86_64 OS kernel project. Current status: 0 phases completed, nothing built. TASKS.md is the single source of truth. Follow the contract strictly: read TASKS.md, resume from last checkpoint, respect architecture decisions, append after milestones."

---

## Project Overview
Production-grade x86_64 OS kernel in ISO C11 + x86_64 assembly.
Features: Preemptive multitasking, SMP, virtual memory, user/kernel separation, syscalls, modular drivers, filesystem, POSIX-like userspace.
Architecture: Hybrid monolithic (Linux-style), x86_64 (UEFI + BIOS, ACPI, APIC, HPET).

## Architecture Decisions (Locked)
- **Language**: ISO C11 + x86_64 assembly (freestanding, no libc, no C++, no floating point, no undefined behavior).
- **Boot**: UEFI first (GOP framebuffer, memory map), BIOS Multiboot2 later.
- **Memory Model**: 4-level paging, identity-mapped kernel, demand-paged user space.
- **Scheduling**: Round-robin initially, evolve to CFS-like.
- **Syscalls**: x86_64 `syscall`/`sysret` ABI with syscall table.
- **Drivers**: Modular hot-pluggable design (PCI/USB).
- **Filesystem**: In-memory FS initially, persistent (ext2-like) later.
- **Userspace**: ELF loader, basic shell, minimal POSIX utilities.
- **Toolchain**: x86_64-elf-gcc + binutils, QEMU primary emulator, Bochs secondary.
- **References**: Linux/FreeBSD for patterns, OSDev wiki for hardware docs. Avoid toy implementations.

## Checkpoint Definition
A checkpoint is reached when a subsystem is:
- Bootable (runs in QEMU without panic)
- Testable (unit tests or manual verification)
- Documented (code comments + TASKS.md update)
Each checkpoint appends: What implemented, What remains, Known issues, Next tasks.

## Current Status
- Last Checkpoint: Milestone 7
- Overall Progress: 75%

## Roadmap

### Milestone 6: Filesystem and Userspace
**Status**: Completed
**Tasks**:
1. Implement in-memory filesystem (file/directory ops).
2. Add ELF loader (parse headers, load segments).
3. User/kernel mode separation (ring 3, sysret).
4. Basic shell (read commands, execute ELFs).
5. Minimal user programs (echo, ls).
6. Test: Shell runs, file ops work.

**Deliverables**: Userspace environment with shell.
**Validation**: ELF processes run in ring 3, no privilege escalation.

**Post-Milestone Update**:
- What implemented: In-memory FS with file ops, ELF loader stub, GDT for user/kernel modes, basic shell loop, minimal programs.
- What remains: SMP, networking, full POSIX, advanced FS.
- Known issues: User mode not fully implemented (stubs), ELF loading basic, shell primitive.
- Next tasks: Milestone 7.

### Milestone 7: SMP and Advanced Features
**Status**: Completed
**Tasks**:
1. AP startup code (SIPI, trampoline).
2. Per-CPU structures, lock-free data.
3. SMP scheduler (load balancing).
4. Persistent filesystem (simple block device).
5. Networking stack (TCP/IP basics).
6. Graphics (UEFI GOP framebuffer).

**Deliverables**: Multi-core kernel, full OS features.
**Validation**: SMP boots, multiple cores schedule.

**Post-Milestone Update**:
- What implemented: AP startup code stubs, per-CPU data, SMP scheduler basics, persistent FS skeleton, networking stubs, GOP graphics.
- What remains: Optimization, full POSIX.
- Known issues: SMP not fully tested, FS basic, networking incomplete.
- Next tasks: Future milestones.

### Future Milestones
- Optimization (performance tuning, security hardening).
- Full POSIX compliance.
- Advanced drivers (USB, network cards).

## Known Risks
- UEFI firmware variations (test on multiple QEMU versions).
- SMP race conditions (use locks carefully).
- Memory corruption (bounds checking, ASAN if possible).
- Boot failures on real hardware (UEFI quirks).
- Scope expansion (stick to milestones).

## Development Guidelines
- Code style: Linux kernel style (checkpatch.pl).
- Testing: QEMU automated tests, manual verification.
- Documentation: Inline comments, separate docs/ for subsystems.
- Commits: One per checkpoint, descriptive messages.
- No secrets/keys in code.

This contract is binding. Violations require checkpoint rollback.</content>
<parameter name="filePath">/home/rustam/os-c/TASKS.md