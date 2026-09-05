#ifndef PAGING_HPP
#define PAGING_HPP
typedef unsigned int uint32_t;

#define PAGE_PRESENT  1
#define PAGE_RW       2

void identity_paging_init(uint32_t pd_phys);

#endif
