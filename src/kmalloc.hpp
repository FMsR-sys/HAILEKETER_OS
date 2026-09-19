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
    
    const uint32_t HEAP_START = 0x01000000;
    const uint32_t HEAP_SIZE  = 16 * 1024 * 1024; //16MB
    
    void init(uint32_t start,uint32_t size);
}

void* kmalloc(uint32_t size);
void kfree(void* ptr);

#endif