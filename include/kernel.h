#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

typedef uint64_t UINTN;
typedef uint32_t UINT32;
typedef uint64_t EFI_PHYSICAL_ADDRESS;
typedef uint64_t EFI_VIRTUAL_ADDRESS;

typedef enum {
    EfiReservedMemoryType,
    EfiLoaderCode,
    EfiLoaderData,
    EfiBootServicesCode,
    EfiBootServicesData,
    EfiRuntimeServicesCode,
    EfiRuntimeServicesData,
    EfiConventionalMemory,
    EfiUnusableMemory,
    EfiACPIReclaimMemory,
    EfiACPIMemoryNVS,
    EfiMemoryMappedIO,
    EfiMemoryMappedIOPortSpace,
    EfiPalCode,
    EfiPersistentMemory,
    EfiMaxMemoryType
} EFI_MEMORY_TYPE;

typedef struct {
    UINT32 Type;
    UINT32 Pad;
    EFI_PHYSICAL_ADDRESS PhysicalStart;
    EFI_VIRTUAL_ADDRESS VirtualStart;
    UINTN NumberOfPages;
    UINTN Attribute;
} EFI_MEMORY_DESCRIPTOR;

void kernel_main(EFI_MEMORY_DESCRIPTOR *MemoryMap, UINTN MapSize, UINTN DescriptorSize, uint64_t kernel_base);

// Kernel initialization functions
void init_gdt();
void smp_init();
void init_processes();
void vfs_init();

// Kernel utilities
void panic(const char *msg);
void *kmalloc(uint64_t size);
void kfree(void *ptr);

// Graphics
void draw_pixel(uint32_t x, uint32_t y, uint32_t color);

// Testing
void run_tests();

#endif