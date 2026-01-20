#!/bin/bash

# OS-C CI Pipeline

echo "Starting CI pipeline..."

# Clean and build
make clean
make
if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi

echo "Build successful"

# Create ESP for QEMU
mkdir -p esp/efi/boot
cp kernel.efi esp/efi/boot/bootx64.efi

# Boot test
echo "Running boot test..."
timeout 10 qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd -drive format=raw,file=fat:rw:esp -serial stdio -nographic -m 512 -smp 1 -no-reboot < /dev/null > boot.log 2>&1

if grep -q "Kernel loaded" boot.log; then
    echo "Boot test: PASS"
else
    echo "Boot test: FAIL"
    cat boot.log
    exit 1
fi

echo "CI pipeline: SUCCESS"