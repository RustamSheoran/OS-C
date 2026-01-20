# OS-C Kernel Debugging Guide

## Kernel Panic Handler
- Triggered by `panic(const char *msg)` in kernel.c
- Prints message to serial console
- Dumps registers (rax, rbx, etc.)
- Generates stack trace
- Halts system

## Stack Trace Generation
- Walks RBP chain in `stack_trace()`
- Prints RIP for each frame
- Handles corrupted stacks gracefully
- Output: `RIP: 0xFFFFFFFF80001234`

## Register Dump
- Captures all GPRs, RIP, RSP, RFLAGS
- SIMD registers optional
- Formatted hex output
- Included in panic reports

## Crash Dump System
- Serial console logging
- Memory region dumps on demand
- Task state inspection
- No persistent storage (volatile)

## GDB Remote Debugging
- `gdbstub.c` implements RSP protocol
- Serial port communication
- Breakpoints, single-step, register access
- Connect: `gdb kernel.elf` then `target remote /dev/ttyUSB0`

## Debug Commands
- Shell `panic` command for testing
- Memory dump: `memdump addr size`
- Register inspect: `regs`
- Stack trace: `trace`

## Debugging Workflow
1. Boot with debug flags
2. Connect GDB if needed
3. Trigger issue
4. Analyze panic output
5. Fix and retest