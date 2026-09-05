#ifndef E820_HPP
#define E820_HPP

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#define E820_BUF        0x5000
#define E820_ENTRY_CNT  0x500

struct e820_record
{
    uint32_t base_low;
    uint32_t base_high;
    uint32_t len_low;
    uint32_t len_high;
    uint32_t type;
    uint32_t attr;
};

namespace e820
{
    void e820_out();
    uint32_t e820_find_first_free(uint32_t min_addr, uint32_t need_size);
    void e820_out_number(uint32_t num);
}

#endif