#include "vga.hpp"
#include "io.hpp"
#include "e820.hpp"
#include "paging.hpp"
#include "pmm.hpp"
#include "pic.hpp"
#include "idt.hpp"
#include "isr.hpp"
#include "kmalloc.hpp"
#include "vmm.hpp"

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

    pmm::init();
    vga::vga_out_string("\npmm init ok!\n");

    vmm::init();
    //--------------------------VMM测试-------------------------
    vga::vga_out_string("\n--- VMM Manual Map Test ---\n");

    const uint32_t test_virt = 0x6000000; //96MB虚拟地址

    uint32_t test_phys = pmm::pages_allocate();
    if(test_phys == 0)
    {
        vga::vga_out_string("PMM allocate test page FAIL!\n");
    }
    else
    {
        vga::vga_out_string("Allocated phys page: ");
        vga::vga_out_sixteen(test_phys);
        vga::vga_out_string("\n");

        bool map_ok = vmm::map(test_virt, test_phys, PAGE_HAVE | PAGE_RW);
        if(!map_ok)
        {
            vga::vga_out_string("vmm::map FAIL, err = ");
            vga::vga_out_sixteen(vmm::err);
            vga::vga_out_string("\n");
        }
        else
        {
            vga::vga_out_string("Map SUCCESS\n");

            uint32_t check_phys = vmm::get_b(test_virt);
            vga::vga_out_string("Virt ");
            vga::vga_out_sixteen(test_virt);
            vga::vga_out_string(" mapped to phys ");
            vga::vga_out_sixteen(check_phys);
            vga::vga_out_string("\n");

            uint8_t* test_ptr = reinterpret_cast<uint8_t*>(test_virt);
            *test_ptr = 0xAA;
            if(*test_ptr == 0xAA)
            {
                vga::vga_out_string("Memory write/read test OK! value=0xAA\n");
            }
            else
            {
                vga::vga_out_string("Memory test FAILED!\n");
            }
            vga::vga_out_string("\n--- VMM unmap test ---\n");
            uint32_t test_virt = 0x06000000;
            if(vmm::unmap(test_virt))
            {
                vga::vga_out_string("unmap SUCCESS\n");
            }
            else
            {
                vga::vga_out_string("unmap FAILED, err:");
                vga::vga_out_sixteen(vmm::err);
                vga::vga_out_sc('\n');
            }
        }
    }
//----------------------------VMM测试结束-----------------------------

    uint32_t mem1 = pmm::pages_allocate();
    vga::vga_out_string("\nAllocated page: ");
    vga::vga_out_sixteen(mem1);
    pmm::pages_release(mem1);
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


    vga::vga_out_string("Heap mark used pages...\n");
    for(uint32_t addr = heap::HEAP_START; addr < heap::HEAP_START + heap::HEAP_SIZE; addr += 4096)
    {
        pmm::mark_used(addr);
    }

    heap::init(heap::HEAP_START, heap::HEAP_SIZE);
    vga::vga_out_string("heap init done!\n");

    char* buf1 = (char*)kmalloc(256);
    vga::vga_out_string("kmalloc buf1: ");
    vga::vga_out_sixteen((uint32_t)buf1);
    buf1[0] = 'H';
    buf1[1] = 'i';
    vga::vga_out_string("\n");

    void* buf2 = kmalloc(512);
    void* buf3 = kmalloc(1024);
    vga::vga_out_string("buf2 = ");
    vga::vga_out_sixteen((uint32_t)buf2);
    vga::vga_out_string(" buf3 = ");
    vga::vga_out_sixteen((uint32_t)buf3);
    vga::vga_out_string("\n");

    vga::vga_out_string("kfree buf2...\n");
    kfree(buf2);

    kfree(buf1);
    kfree(buf3);
    vga::vga_out_string("All free finished!\nHeap test OK.\n");

    asm volatile("sti");

    while(true)
    {
        asm("hlt");
    }
}
