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
        /*set(0,(uint32_t)isr0,0x08,0x8E);
        set(1,(uint32_t)isr1,0x08,0x8E);
        set(2,(uint32_t)isr1,0x08,0x8E);
        set(3,(uint32_t)isr1,0x08,0x8E);
        set(4,(uint32_t)isr1,0x08,0x8E);
        set(5,(uint32_t)isr1,0x08,0x8E);
        set(6,(uint32_t)isr1,0x08,0x8E);
        set(7,(uint32_t)isr1,0x08,0x8E);
        set(8,(uint32_t)isr1,0x08,0x8E);
        set(9,(uint32_t)isr1,0x08,0x8E);
        set(10,(uint32_t)isr1,0x08,0x8E);
        set(11,(uint32_t)isr1,0x08,0x8E);
        set(12,(uint32_t)isr1,0x08,0x8E);
        set(13,(uint32_t)isr1,0x08,0x8E);
        set(14,(uint32_t)isr1,0x08,0x8E);
        set(15,(uint32_t)isr1,0x08,0x8E);
        set(16,(uint32_t)isr1,0x08,0x8E);
        set(17,(uint32_t)isr1,0x08,0x8E);
        set(18,(uint32_t)isr1,0x08,0x8E);
        set(19,(uint32_t)isr1,0x08,0x8E);
        set(20,(uint32_t)isr1,0x08,0x8E);*/
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