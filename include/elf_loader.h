#ifndef ELF_LOADER_H
#define ELF_LOADER_H

#include <stdint.h>

uint64_t load_elf(void *elf_data, uint64_t *entry);

#endif