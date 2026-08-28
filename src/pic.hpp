#ifndef PIC_HPP
#define PIC_HPP

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;


namespace pic
{
    void remap();
    void send_eoi(uint8_t irq);
}

#endif
