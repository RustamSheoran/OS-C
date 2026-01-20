# OS-C Build Guide

## Prerequisites
- Linux host (Ubuntu/Debian recommended)
- x86_64-elf-gcc toolchain
- binutils for x86_64-elf
- QEMU with UEFI support (OVMF)
- Git for version control

## Toolchain Setup
Download prebuilt toolchain:
```bash
wget https://github.com/lordmilko/i686-elf-tools/releases/download/13.2.0/x86_64-elf-tools-linux64.tar.xz
tar xf x86_64-elf-tools-linux64.tar.xz
export PATH=$PWD/x86_64-elf-tools/bin:$PATH
```

Or build from source (GCC, binutils for x86_64-elf target).

## Dependencies
```bash
sudo apt update
sudo apt install qemu qemu-kvm ovmf build-essential
```

## Building
```bash
git clone https://github.com/RustamSheoran/OS-C.git
cd OS-C
make
```

This produces:
- `kernel.elf`: ELF executable
- `kernel.efi`: UEFI PE image

## Running
```bash
./scripts/run.sh
```

Opens QEMU with UEFI firmware, boots kernel to serial shell.

## Testing
```bash
make test  # Build test kernel
./scripts/run_test.sh  # Run automated tests
```

## Debugging
```bash
make debug  # Build with debug symbols
gdb kernel.elf
target remote /dev/ttyUSB0  # After QEMU starts
```

## CI Pipeline
Run `./scripts/ci.sh` for automated build + boot test.

## Troubleshooting
- Build fails: Check toolchain PATH
- No serial output: Verify QEMU OVMF path
- Boot hangs: Enable QEMU debug options