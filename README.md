# OS-C: x86_64 Operating System Kernel

A production-grade x86_64 operating system kernel written in ISO C11 and x86_64 assembly. This kernel targets real hardware with UEFI boot support and implements modern OS features.

## Features

- **UEFI Boot**: Boots from UEFI firmware with memory map parsing and higher-half kernel mapping.
- **Preemptive Multitasking**: Round-robin scheduler with context switching, timer-driven preemption.
- **SMP Support**: Symmetric multiprocessing with AP startup via SIPI, per-CPU data structures.
- **Virtual Memory**: 4-level paging, demand-paged user space, kernel identity mapping.
- **Memory Management**: Bitmap-based physical memory allocator, bump heap allocator.
- **Interrupt Handling**: IDT setup, PIC remapping, PIT timer, syscall interface.
- **Filesystem**: In-memory filesystem with file operations, ATA PIO disk driver.
- **Networking**: RTL8139 Ethernet driver stub, TCP/IP stack placeholders.
- **Graphics**: UEFI GOP framebuffer support, pixel drawing primitives.
- **Security**: Basic error handling, panic recovery, syscall validation.
- **POSIX Compatibility**: Syscall interface modeled after POSIX standards.

## Architecture

- **Language**: ISO C11 + x86_64 assembly (freestanding, no libc)
- **Boot**: UEFI first, BIOS fallback
- **Memory Model**: 4-level paging, higher-half kernel
- **Scheduling**: Preemptive round-robin
- **Syscalls**: x86_64 syscall/sysret ABI
- **Drivers**: Modular design for PCI/USB devices

## Building

### Prerequisites

- x86_64-elf-gcc toolchain (binutils, gcc)
- QEMU for emulation
- OVMF for UEFI firmware

Install toolchain (example for Ubuntu):

```bash
# Build or download x86_64-elf-gcc
# For prebuilt: wget https://github.com/lordmilko/i686-elf-tools/releases/download/13.2.0/x86_64-elf-tools-linux64.tar.xz
# tar xf x86_64-elf-tools-linux64.tar.xz
# export PATH=$PWD/x86_64-elf-tools/bin:$PATH
```

### Compile

```bash
make
```

This produces `kernel.efi`.

### Run in QEMU

```bash
./scripts/run.sh
```

Requires OVMF:

```bash
sudo apt install ovmf
```

## Usage

The kernel boots to a serial console shell with commands:

- `ls`: List files
- `echo hello`: Print message
- `cat test.txt`: Read file
- `uptime`: Show system uptime
- `meminfo`: Memory statistics
- `draw`: Draw a pixel
- `help`: Show commands

## Project Structure

- `src/`: Source code
- `include/`: Headers
- `scripts/`: Build scripts
- `TASKS.md`: Development contract (ignored in repo)

## Contributing

1. Fork the repository
2. Create a feature branch
3. Implement changes
4. Test with QEMU
5. Submit pull request

## License

This project is open-source. See LICENSE for details.

## Roadmap

- Full filesystem implementation
- TCP/IP stack
- GUI desktop
- POSIX compliance
- Hardware driver support

## Contact

Repository: https://github.com/RustamSheoran/OS-C