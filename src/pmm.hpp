#ifndef PMM_HPP
#define PMM_HPP

#include "vga.hpp"

#define PAGE_SIZE 4096

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

namespace pmm
{
    void init();
    uint32_t pages_allocate();
    void pages_release(uint32_t phys_addr);
    void mark_used(uint32_t phys_addr);
    void mark_free(uint32_t phys_addr);
    bool is_used(uint32_t phys_addr);
    bool is_free(uint32_t phys_addr);
}

#endif