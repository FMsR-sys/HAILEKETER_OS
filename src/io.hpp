#ifndef IO_HPP
#define IO_HPP

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

namespace io
{
    uint8_t read_uint8 (uint16_t port);
    void write_uint8 (uint16_t port , uint8_t value);    //port端口号，value要写的内容
    uint16_t read_uint16 (uint16_t port);
    void write_uint16 (uint16_t port , uint16_t value);
    uint32_t read_uint32 (uint16_t port);
    void write_uint32 (uint16_t port , uint32_t value);
    void wait ();

}

#endif