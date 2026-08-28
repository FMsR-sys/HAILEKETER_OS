#ifndef IO_HPP
#define IO_HPP

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;

namespace io
{
    uint8_t inb(uint16_t port);
    void outb(uint16_t port, uint8_t val);
}

#endif
