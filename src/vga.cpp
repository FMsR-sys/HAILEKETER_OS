#include "vga.hpp"

uint16_t* vga::VGA_BUFFER = reinterpret_cast<uint16_t*>(0xB8000);
uint8_t vga::color = 0x1E;
uint8_t vga::cursor_x = 0;
uint8_t vga::cursor_y = 0;

void vga::vga_out_sc(int x,int y,char sc)
{
    int idx = y * vga::VGA_WIDE + x;
    vga::VGA_BUFFER[idx] = static_cast<uint16_t>(sc) | (static_cast<uint16_t>(vga::color) << 8);
}

void vga::vga_out_string(const char* st)
{
    while (*st != '\0')
    {
        if(*st == '\n')
        {
            vga::cursor_y++;
            vga::cursor_x = 0;
            st++;
        }
        else
        {
            vga::vga_out_sc(vga::cursor_x,vga::cursor_y,*st);
            st++;
            vga::cursor_x++;
        }

        if (vga::cursor_x >= vga::VGA_WIDE)
        {
            vga::cursor_x = 0;
            vga::cursor_y++;
        }
        while (vga::cursor_y >= vga::VGA_HIGH)
        {
            vga::vga_screen_up();
            vga::cursor_y--;
            vga::cursor_x = 0;
        }
    }
}


void vga::vga_clean()
{
    uint16_t* buf = reinterpret_cast<uint16_t*>(vga::VGA_BUFFER);
    for(int i = 0; i < vga::VGA_WIDE * vga::VGA_HIGH; i++)
    {
        buf[i] = (static_cast<uint16_t>(vga::color) << 8) | ' ';
    }
    vga::cursor_x = 0;
    vga::cursor_y = 0;
}

void vga::vga_screen_up()
{
    for (int y = 0;y < vga::VGA_HIGH - 1;y++)
    {
        for (int x = 0;x < vga::VGA_WIDE;x++)
        {
            int src_index = (y + 1) * vga::VGA_WIDE + x;
            int dest_index = y * vga::VGA_WIDE + x;
            vga::VGA_BUFFER[dest_index] = vga::VGA_BUFFER[src_index];
        }
    }

    int bottom_y = vga::VGA_HIGH - 1;
    for(int x = 0; x < vga::VGA_WIDE; x++)
    {
        int idx = bottom_y * vga::VGA_WIDE + x;
        vga::VGA_BUFFER[idx] = (static_cast<uint16_t>(vga::color) << 8) | ' ';
    }
}

uint8_t vga::vga_color(uint8_t bg,uint8_t ps)
{
    uint8_t c = (bg << 4) | ps;
    vga::vga_set_color(c);
    return (bg << 4) | ps;
}

void vga::vga_set_color(uint8_t set)
{
    vga::color = set;
}

void vga::vga_move_cursor(int x,int y)
{
    vga::cursor_x = x;
    vga::cursor_y = y;
}