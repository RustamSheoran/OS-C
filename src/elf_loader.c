#include <string.h>
#include "elf.h"
#include "pmm.h"
#include "paging.h"
#include "kernel.h"

uint64_t load_elf(void *elf_data, uint64_t *entry) {
    Elf64_Ehdr *hdr = (Elf64_Ehdr *)elf_data;
    if (hdr->e_ident[0] != 0x7F || hdr->e_ident[1] != 'E' || hdr->e_ident[2] != 'L' || hdr->e_ident[3] != 'F') {
        panic("Invalid ELF");
    }
    *entry = hdr->e_entry;
    Elf64_Phdr *phdr = (Elf64_Phdr *)((uint8_t *)elf_data + hdr->e_phoff);
    for (int i = 0; i < hdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            uint64_t vaddr = phdr[i].p_vaddr;
            uint64_t memsz = phdr[i].p_memsz;
            uint64_t filesz = phdr[i].p_filesz;
            uint64_t flags = 4; // User
            if (phdr[i].p_flags & PF_R) flags |= 1;
            if (phdr[i].p_flags & PF_W) flags |= 2;
            // Allocate pages
            for (uint64_t addr = vaddr & ~0xFFF; addr < (vaddr + memsz + 0xFFF) & ~0xFFF; addr += 0x1000) {
                uint64_t phys = pmm_alloc_page();
                map_page(addr, phys, flags);
            }
            // Copy data
            memcpy((void *)vaddr, (uint8_t *)elf_data + phdr[i].p_offset, filesz);
            // Zero rest
            memset((uint8_t *)vaddr + filesz, 0, memsz - filesz);
        }
    }
    return 0;
}