#!/bin/bash
set -euo pipefail

make

mkdir -p esp/efi/boot
cp kernel.efi esp/efi/boot/bootx64.efi

OVMF_FW=""
for p in \
    /usr/share/ovmf/OVMF.fd \
    /usr/share/OVMF/OVMF.fd \
    /usr/share/OVMF/OVMF_CODE.fd \
    /usr/share/edk2/ovmf/OVMF.fd
do
    if [ -f "$p" ]; then
        OVMF_FW="$p"
        break
    fi
done

if [ -z "$OVMF_FW" ]; then
    echo "OVMF firmware not found" >&2
    exit 1
fi

qemu-system-x86_64 \
    -bios "$OVMF_FW" \
    -drive format=raw,file=fat:rw:esp \
    -serial stdio \
    -nographic \
    -m 512 \
    -smp 1
