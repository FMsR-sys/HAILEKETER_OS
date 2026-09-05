#include "io.hpp"

uint8_t io::read_uint8(uint16_t port)
{
    uint8_t result;
    asm volatile("inb %1, %0"
                 : "=a"(result)
                 : "Nd"(port));
    return result;
}

void io::write_uint8(uint16_t port, uint8_t value)
{
    asm volatile(
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

uint16_t io::read_uint16(uint16_t port)
{
    uint16_t result;
    asm volatile("inw %1, %0"
                 : "=a"(result)
                 : "Nd"(port));
    return result;
}

void io::write_uint16(uint16_t port, uint16_t value)
{
    asm volatile(
        "outw %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

uint32_t io::read_uint32(uint16_t port)
{
    uint32_t result;
    asm volatile("inl %1, %0"
                 : "=a"(result)
                 : "Nd"(port));
    return result;
}

void io::write_uint32 (uint16_t port , uint32_t value)
{
    asm volatile(
        "outl %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

void io::wait()
{
    write_uint8(0x80, 0);
}
