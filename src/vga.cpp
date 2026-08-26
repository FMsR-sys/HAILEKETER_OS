#include "vga.hpp"

uint16_t* VGA_BUFFER = reinterpret_cast<uint16_t*>(0xB8000);
const uint8_t OUT_COLOR = 0x1E;
uint8_t cursor_x = 0;
uint8_t cursor_y = 0;

void vga_out_sc(int x, int y, char sc)
{
    int idx = y * VGA_WIDE + x;
    VGA_BUFFER[idx] = static_cast<uint16_t>(sc) | (static_cast<uint16_t>(OUT_COLOR) << 8);
}

void vga_out_string(int x, int y, const char* st)
{
    while (*st != '\0')
    {
        vga_out_sc(x,y,*st);
        x++;
        st++;
    }
}
