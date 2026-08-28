#include "io.hpp"

extern "C" uint8_t io_inb(uint16_t port);
extern "C" void io_outb(uint16_t port, uint8_t val);

namespace io
{
    uint8_t inb(uint16_t port)
    {
        return io_inb(port);
    }

    void outb(uint16_t port, uint8_t val)
    {
        io_outb(port, val);
    }
}
