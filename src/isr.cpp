#include "isr.hpp"
#include "pic.hpp"
#include "io.hpp"
#include "vga.hpp"

extern "C" void isr_handler(uint8_t int_no)
{
    if(int_no == 0x21)
    {
        uint8_t scancode = io::inb(0x60);
        // vga::vga_out_string("Key pressed! ");
    }
    pic::send_eoi(int_no - 0x20);
}
