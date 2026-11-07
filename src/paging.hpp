#ifndef PAGING_HPP
#define PAGING_HPP
typedef unsigned int uint32_t;

#define PAGE_PRESENT  1

// mem_end: 可用物理内存上界（会被 4K 对齐）
void identity_paging_init(uint32_t mem_end);
void identity_map_range(uint32_t start, uint32_t end);
uint32_t* identity_paging_dir();

#endif
