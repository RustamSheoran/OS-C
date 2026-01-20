# OS-C Architecture Lock

## Core Principles
- Freestanding C11, no libc
- x86_64 syscall ABI
- UEFI boot first
- Hybrid monolithic design
- Higher-half kernel
- Round-robin preemptive scheduler
- Modular driver framework

## Locked Decisions
- Memory: 4-level paging, bitmap PFA
- Scheduling: Timer-driven, context switching
- Syscalls: x86_64 native, handler in assembly
- I/O: Polled serial, interrupt-driven others
- FS: In-memory with ATA backend
- SMP: SIPI for AP boot, shared scheduler
- Security: NX, ASLR planned
- Testing: Kernel self-tests, QEMU automation

## Change Control
All changes must:
- Maintain ABI compatibility
- Pass existing tests
- Update documentation
- Be reviewed for security

## Release Criteria
- All tests pass
- No regressions
- Hardware validation complete
- Documentation updated

This file is locked. Changes require consensus.