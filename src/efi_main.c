#include <stdint.h>
#include <stddef.h>
#include "efi.h"

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    EFI_GUID loaded_image_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    EFI_LOADED_IMAGE_PROTOCOL *loaded_image = NULL;
    EFI_STATUS Status = SystemTable->BootServices->HandleProtocol(
        ImageHandle,
        &loaded_image_guid,
        (void **)&loaded_image
    );
    uint64_t kernel_base = 0;
    if (Status == EFI_SUCCESS && loaded_image != NULL) {
        kernel_base = (uint64_t)loaded_image->ImageBase;
    }

    UINTN MapSize = 0;
    EFI_MEMORY_DESCRIPTOR *MemoryMap = NULL;
    UINTN MapKey = 0;
    UINTN DescriptorSize = 0;
    UINT32 DescriptorVersion = 0;

    Status = SystemTable->BootServices->GetMemoryMap(
        &MapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion
    );
    if (Status != EFI_BUFFER_TOO_SMALL) {
        while (1) { }
    }

    MapSize += DescriptorSize * 8;
    UINTN Pages = (MapSize + 0xFFFULL) / 0x1000ULL;
    EFI_PHYSICAL_ADDRESS MapAddr = 0;
    Status = SystemTable->BootServices->AllocatePages(
        EFI_ALLOCATE_ANY_PAGES, EfiLoaderData, Pages, &MapAddr
    );
    if (Status != EFI_SUCCESS) {
        while (1) { }
    }
    MemoryMap = (EFI_MEMORY_DESCRIPTOR *)(uintptr_t)MapAddr;

    while (1) {
        UINTN CurrentMapSize = MapSize;
        Status = SystemTable->BootServices->GetMemoryMap(
            &CurrentMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion
        );
        if (Status == EFI_BUFFER_TOO_SMALL) {
            while (1) { }
        }
        if (Status != EFI_SUCCESS) {
            while (1) { }
        }

        Status = SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);
        if (Status == EFI_SUCCESS) {
            MapSize = CurrentMapSize;
            break;
        }
    }

    // Jump to kernel
    extern void kernel_main(EFI_MEMORY_DESCRIPTOR *MemoryMap, UINTN MapSize, UINTN DescriptorSize, uint64_t kernel_base);
    kernel_main(MemoryMap, MapSize, DescriptorSize, kernel_base);

    // Should not return
    while (1) { }
    return EFI_SUCCESS;
}
