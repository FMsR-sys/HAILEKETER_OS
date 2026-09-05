#include "idt.hpp"

namespace idt
{
    static idt_entry table[256];
    static idt_ptr pointer;

    void init()
    {
        for (int i = 0;i < 256;i++)
        {
            table[i].offest_low = 0;
            table[i].selector = 0;
            table[i].zero = 0;
            table[i].flags = 0;
            table[i].offest_high = 0;
        }
        pointer.limit = sizeof(table) - 1;
        pointer.base  = (uint32_t)&table;
        asm volatile ("lidt (%0)"
                      :
                      :"r" (&pointer));
    }

    void set(uint8_t index , uint32_t offset , uint16_t sel , uint8_t flags)
    {
        table[index].offest_low = offset & 0xFFFFU;
        table[index].selector = sel;
        table[index].zero = 0;
        table[index].flags = flags;
        table[index].offest_high = (offset >> 16) & 0xFFFFU;
    }
}