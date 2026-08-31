#include "e820.hpp"
#include "vga.hpp"

#define E820_TABLE_ADDR   0x5000
#define E820_COUNT_ADDR   0x500
#define E820_ENTRY_SIZE   24

namespace e820
{
    void e820_out()
    {
        uint16_t count = *(reinterpret_cast<uint16_t*>(E820_COUNT_ADDR));
        vga::vga_out_string("E820 Memory Map:\n");

        for (int i = 0; i < count; i++)
        {
            e820_record* rec = reinterpret_cast<e820_record*>(E820_TABLE_ADDR + i * E820_ENTRY_SIZE);

            vga::vga_out_string("Base: ");
            e820_out_number(rec->base_low);
            vga::vga_out_string("  Type: ");
            e820_out_number(rec->type);
            vga::vga_out_string("\n");
        }
    }

    void e820_out_number(uint32_t num)
    {
        const char hex_table[] = "0123456789ABCDEF";
        char buf[9];
        buf[8] = '\0';
        for (int i = 7; i >= 0; i--)
        {
            uint32_t four_bit = (num >> (i * 4)) & 0xF;
            buf[i] = hex_table[four_bit];
        }
        vga::vga_out_string(buf);
    }

    uint32_t e820_find_first_free(uint32_t min_addr, uint32_t need_size)
    {
        uint16_t count = *(reinterpret_cast<uint16_t*>(E820_COUNT_ADDR));

        for (int i = 0; i < count; i++)
        {
            e820_record* ent = reinterpret_cast<e820_record*>(
                E820_BUF + i * E820_ENTRY_SIZE);

            if (ent->type == 1 && ent->base_high == 0)
            {
                uint32_t base = ent->base_low;
                uint32_t size = ent->len_low;
                uint32_t end = base + size;

                if (base < min_addr)
                {
                    base = min_addr;
                }

                if (base >= end)
                {
                    continue;
                }

                if ((end - base) >= need_size)
                {
                    return base;
                }
            }
        }
        return 0;
    }

}
