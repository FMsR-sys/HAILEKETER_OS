#include "vga.hpp"

extern "C" void kernel_main()
{
    extern unsigned int __bss_start;
    extern unsigned int __bss_end;

    unsigned int *ptr = &__bss_start;
    unsigned int *end = &__bss_end;

    for (; ptr < end; ptr++)
    {
        *ptr = 0;
    }

    vga_out_string(5,5,"Hello World!");

    while(true)
    {
        asm("hlt");
    }
}
