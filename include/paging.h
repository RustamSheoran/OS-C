#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

void paging_init(uint64_t kernel_base);
void map_page(uint64_t virt, uint64_t phys, uint64_t flags);

#endif