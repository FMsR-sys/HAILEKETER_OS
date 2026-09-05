#ifndef PIC_HPP
#define PIC_HPP

#include "io.hpp"

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;


namespace pic
{
    void remap (uint8_t main_off , uint8_t slave_off);
    void mask (uint8_t irq);
    void unmask (uint8_t irq);
    void eoi (uint8_t irq);
}

#endif