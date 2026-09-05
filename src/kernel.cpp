#include "vga.hpp"
#include "io.hpp"
#include "e820.hpp"
#include "paging.hpp"
#include "pmm.hpp"
#include "pic.hpp"
#include "idt.hpp"
#include "isr.hpp"

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

    vga::vga_clean();
    e820::e820_out();
    uint32_t pd_addr = 0x200000;
    vga::vga_out_string("\nGoing to turn paging on...");
    identity_paging_init(pd_addr);
    vga::vga_out_string("\nPaging ON!");
    pmm::pmm_init();
    vga::vga_out_string("\npmm init ok!\n");
    uint32_t mem1 = pmm::pmm_pages_allocate();
    vga::vga_out_string("Allocated page: ");
    vga::vga_out_sixteen(mem1);
    pmm::pmm_pages_release(mem1);
    vga::vga_out_string("\nReleased!\n");
    uint8_t pic_mask = io::read_uint8(0x21);
    vga::vga_out_string("IO test, PIC mask = ");
    vga::vga_out_sixteen(pic_mask);
    vga::vga_out_string("\nGoing to trun remap...\n");
    pic::remap(0x20, 0x28);
    vga::vga_out_string("remap finish!\n");
    idt::init();
    idt::set(0x21, (uint32_t)irq1_stub, 0x08, 0x8E);
    pic::unmask(1);
    vga::vga_out_string("idt finish!\n");
    asm volatile("sti");

    while(true)
    {
        asm("hlt");
    }
}
