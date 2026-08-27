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

    vga::vga_color(0,15);
    vga::vga_clean();
    vga::vga_out_string(
    "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n"
    "11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n"
    "21\n22\n23\n24\n25\n26\n27\n28\n");

    while(true)
    {
        asm("hlt");
    }
}
