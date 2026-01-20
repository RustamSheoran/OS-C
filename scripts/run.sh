#!/bin/bash

# Build the kernel
make

# Create ESP directory
mkdir -p esp/efi/boot
cp kernel.efi esp/efi/boot/bootx64.efi

# Run QEMU
qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd -drive format=raw,file=fat:rw:esp -serial stdio -nographic