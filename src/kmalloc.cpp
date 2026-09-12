#include "kmalloc.hpp"

static heap_block* heap_head = nullptr;

namespace heap
{
    void init(uint32_t start,uint32_t size)
    {
        heap_head = reinterpret_cast<heap_block*>(start);
        heap_head -> size = size - sizeof(heap_block);
        heap_head -> used = false;
        heap_head -> next = nullptr;
    }

    static uint32_t align4(uint32_t n)
    {
        if (n % 4 == 0)
        {
            return n;
        }
        return n + (4 - n % 4);
    }
}

void* kmalloc(uint32_t size)
    {
        if (heap_head == nullptr)
        {
            return nullptr;
        }

        uint32_t need = heap::align4(size);
        heap_block* blk = heap_head;

        while (blk != nullptr)
        {
            if (!blk -> used && blk -> size >= need)
            {
                uint32_t a = blk -> size - need - sizeof(heap_block);
                if (a > sizeof(heap_block))
                {
                    heap_block* new_block = reinterpret_cast<heap_block*>(reinterpret_cast<uint8_t*>(blk + 1) + need);
                    new_block -> size = a;
                    new_block -> used = false;
                    new_block -> next = blk -> next;
                    blk -> size = need;
                    blk -> next = new_block;
                }
                blk -> used = true;
                return blk + 1;
            }
            blk = blk -> next;
        }
        return nullptr;
    }

    void kfree(void* ptr)
    {
        if (ptr == nullptr)
        {
            return;
        }

        heap_block* blk = reinterpret_cast<heap_block*>(ptr) - 1;

        if (blk->used == false)
        {
            vga::vga_out_string("double free detected!\n");
            return;
        }

        blk -> used = false;
        heap_block* prev_blk = nullptr;
        heap_block* curr = heap_head;

        while (curr != nullptr && curr -> next != blk)
        {
            prev_blk = curr;
            curr = curr -> next;
        }

        heap_block* next_block = blk -> next;
        
        if (next_block != nullptr && !next_block -> used)
        {
            blk -> size += next_block -> size + sizeof(heap_block);
            blk -> next = next_block -> next;
        }
        if (prev_blk != nullptr && !prev_blk -> used)
        {
            prev_blk -> size += blk -> size + sizeof(heap_block);
            prev_blk -> next = blk -> next;
        }
    }