# OS-C Hardware Compatibility

## Tested Platforms

### Tier 1: Full Support
- **QEMU 6.0+ with UEFI (OVMF)**: Primary development platform
  - Full feature support including SMP, networking, graphics
  - Automated CI testing
  - Known working: -smp 1-4, -m 512MB+, RTL8139, ATA drives

### Tier 2: Basic Boot Support
- **VirtualBox 7.0+**: EFI boot with basic functionality
- **VMware Workstation**: EFI compatibility
- **Real Hardware**: Intel Core i5/i7 (Broadwell+)
  - Requires UEFI firmware
  - Serial console for debugging
  - May need custom OVMF builds

### Tier 3: Known Limitations
- **AMD Ryzen**: LAPIC/APIC issues (mitigated in code)
- **Older QEMU (<6.0)**: UEFI memory map bugs
- **Legacy BIOS**: No support (UEFI only)
- **ARM/x86-32**: Not supported (x86_64 only)

## Hardware Requirements

### Minimum
- x86_64 CPU with UEFI support
- 512MB RAM
- Serial port (COM1) for console
- UEFI firmware (OVMF recommended)

### Recommended
- Multi-core CPU for SMP testing
- 1GB+ RAM
- ATA/IDE or NVMe storage
- Ethernet card (RTL8139 compatible)

## Known Issues & Workarounds

### QEMU Compatibility
- **Memory Map Issues**: Use OVMF 2023+ or custom builds
- **SMP Boot**: Ensure -smp parameter matches APIC configuration
- **Network**: RTL8139 works, others may need driver updates

### Real Hardware
- **UEFI Variables**: Some firmwares don't support runtime services
- **Secure Boot**: Disabled for custom kernels
- **ACPI Issues**: Basic ACPI parsing, may need updates
- **Graphics**: GOP framebuffer may vary by GPU

### Driver Compatibility
- **ATA**: PIO mode only, no DMA yet
- **NVMe**: Basic command queues, no advanced features
- **RTL8139**: Init and basic I/O, no advanced networking
- **GOP**: Framebuffer access, no acceleration

## Testing Matrix

| Feature | QEMU | VirtualBox | Real HW |
|---------|------|------------|---------|
| UEFI Boot | ✅ | ✅ | ✅ |
| SMP | ✅ | ⚠️ | ✅ |
| Memory Mgmt | ✅ | ✅ | ✅ |
| Syscalls | ✅ | ✅ | ✅ |
| ATA Storage | ✅ | ⚠️ | ✅ |
| NVMe Storage | ✅ | ❌ | ⚠️ |
| Networking | ✅ | ❌ | ⚠️ |
| Graphics | ✅ | ⚠️ | ⚠️ |
| Serial Console | ✅ | ✅ | ✅ |

✅ = Fully supported
⚠️ = Partially supported
❌ = Not supported

## Hardware Validation Process

1. **Boot Test**: Kernel loads and initializes
2. **Subsystem Test**: Run `test` command in shell
3. **I/O Test**: Verify storage and network devices
4. **Stress Test**: Memory allocation and task creation
5. **Compatibility Report**: Document any issues

## Adding New Hardware Support

1. **Identify Hardware**: PCI/PCIe device IDs
2. **Implement Driver**: Follow existing driver patterns
3. **Add Tests**: Hardware-specific validation
4. **Update Compatibility**: Add to this document
5. **CI Integration**: Test on supported platforms

## Future Hardware Support

- **USB Controllers**: UHCI/EHCI/xHCI drivers
- **Advanced Storage**: AHCI SATA, RAID controllers
- **Network Cards**: Intel e1000, Broadcom BCM57xx
- **Graphics**: Intel i915, AMD Radeon basic modes
- **Audio**: Intel HDA, AC97 compatibility
- **Input Devices**: PS/2, USB HID

## Firmware Requirements

- **UEFI 2.4+**: Required for memory map and GOP
- **ACPI 2.0+**: Basic table parsing
- **SMBIOS**: Optional system information
- **Secure Boot**: Disabled (not supported)

## Debugging Hardware Issues

- **Serial Logs**: All debug output goes to COM1
- **QEMU Debug**: Use `-d` flags for detailed logging
- **Hardware Registers**: Use `lspci`, `lsusb` for device info
- **UEFI Shell**: Boot into firmware shell for diagnostics