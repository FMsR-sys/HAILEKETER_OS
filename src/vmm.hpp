#ifndef VMM_HPP
#define VMM_HPP

#include "pmm.hpp"

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#define PAGE_PRESENT    (1U << 0)
#define PAGE_RW         (1U << 1)
#define PAGE_USER       (1U << 2)

struct page_dir_entry
{
    uint32_t value;
};

struct page_table_entry
{
    uint32_t value;
};

constexpr uint32_t PD_ENTRY_COUNT = 1024;
constexpr uint32_t PT_ENTRY_COUNT = 1024;

namespace vmm
{
    void init();
    bool ma_page(uint32_t vddr,uint32_t flags); //把flags物理页分配到vddr
    void switch_cr3(page_dir_entry* pd);
    uint32_t virt_to_phys(uint32_t vaddr);
}

#endif