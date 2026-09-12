#ifndef KMALLOC_HPP
#define KMALLOC_HPP

#include "pmm.hpp"

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

struct heap_block
{
    uint32_t size;
    bool used;    //true=被分配，false=空闲
    heap_block* next;

};


namespace heap
{
    void init(uint32_t start,uint32_t size);
}

void* kmalloc(uint32_t size);
void kfree(void* ptr);

#endif