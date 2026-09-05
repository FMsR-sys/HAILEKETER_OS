#ifndef PMM_HPP
#define PMM_HPP

#include "vga.hpp"

#define PAGE_SIZE 4096

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

namespace pmm
{
    void pmm_init();
    uint32_t pmm_pages_allocate();
    void pmm_pages_release(uint32_t phys_addr);
    void pmm_mark_used(uint32_t phys_addr);
    void pmm_mark_free(uint32_t phys_addr);
    bool pmm_is_used(uint32_t phys_addr);
    bool pmm_is_free(uint32_t phys_addr);
}

#endif