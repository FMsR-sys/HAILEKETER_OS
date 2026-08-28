#include "pic.hpp"
#include "io.hpp"

#define PIC_MASTER_CMD     0x20
#define PIC_MASTER_DATA    0x21
#define PIC_SLAVE_CMD      0xA0
#define PIC_SLAVE_DATA     0xA1

void pic::remap()
{
    uint8_t mask_master = io::inb(PIC_MASTER_DATA);
    uint8_t mask_slave  = io::inb(PIC_SLAVE_DATA);

    io::outb(PIC_MASTER_CMD, 0x11);
    io::outb(PIC_SLAVE_CMD,  0x11);

    io::outb(PIC_MASTER_DATA, 0x20);
    io::outb(PIC_SLAVE_DATA,  0x28);

    io::outb(PIC_MASTER_DATA, 0x04);
    io::outb(PIC_SLAVE_DATA,  0x02);

    io::outb(PIC_MASTER_DATA, 0x01);
    io::outb(PIC_SLAVE_DATA,  0x01);

    io::outb(PIC_MASTER_DATA, mask_master);
    io::outb(PIC_SLAVE_DATA,  mask_slave);
}

void pic::send_eoi(uint8_t irq)
{
    if(irq >= 8)
    {
        io::outb(PIC_SLAVE_CMD, 0x20);
    }
    io::outb(PIC_MASTER_CMD, 0x20);
}
