#include <stdint.h>
#include <stddef.h>
#include "efi.h"

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    // Get memory map size first
    UINTN MapSize = 0;
    EFI_MEMORY_DESCRIPTOR *MemoryMap = NULL;
    UINTN MapKey;
    UINTN DescriptorSize;
    UINT32 DescriptorVersion;

    EFI_STATUS Status = SystemTable->BootServices->GetMemoryMap(&MapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    if (Status != EFI_BUFFER_TOO_SMALL) {
        // Error
        while (1);
    }

    // Allocate pages for memory map
    UINTN Pages = (MapSize + 0xFFF) / 0x1000; // Align to page
    EFI_PHYSICAL_ADDRESS MapAddr;
    Status = SystemTable->BootServices->AllocatePages(EFI_ALLOCATE_ANY_PAGES, EfiLoaderData, Pages, &MapAddr);
    if (Status != EFI_SUCCESS) {
        while (1);
    }
    MemoryMap = (EFI_MEMORY_DESCRIPTOR *)MapAddr;

    // Get the memory map
    Status = SystemTable->BootServices->GetMemoryMap(&MapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    if (Status != EFI_SUCCESS) {
        while (1);
    }

    // Exit boot services
    Status = SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);
    if (Status != EFI_SUCCESS) {
        while (1);
    }

    // Get kernel image base
    EFI_GUID loaded_image_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    EFI_LOADED_IMAGE_PROTOCOL *loaded_image;
    EFI_STATUS Status2 = SystemTable->BootServices->HandleProtocol(ImageHandle, &loaded_image_guid, (void **)&loaded_image);
    uint64_t kernel_base = 0;
    if (Status2 == EFI_SUCCESS) {
        kernel_base = (uint64_t)loaded_image->ImageBase;
    }

    // Jump to kernel
    extern void kernel_main(EFI_MEMORY_DESCRIPTOR *MemoryMap, UINTN MapSize, UINTN DescriptorSize, uint64_t kernel_base);
    kernel_main(MemoryMap, MapSize, DescriptorSize, kernel_base);

    // Should not return
    while (1);
    return EFI_SUCCESS;
}