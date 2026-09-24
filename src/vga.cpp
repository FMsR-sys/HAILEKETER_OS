#include "vga.hpp"
#include "io.hpp"
uint16_t* vga::VGA_BUFFER = reinterpret_cast<uint16_t*>(0xB8000);
uint8_t vga::color = 0x1E;
uint8_t vga::cursor_x = 0;
uint8_t vga::cursor_y = 0;

#define BACK_ROWS 512
uint16_t back_buffer[BACK_ROWS][512];
uint32_t view_offset = 0;
uint32_t write_line = 0;
uint32_t write_col = 0;

static void vga_render(void)
{
    for(uint32_t y = 0; y < 25; y++)
    {
        uint32_t src_y = view_offset + y;
        for(uint32_t x = 0; x < 80; x++)
        {
            uint32_t dst_idx = y * 80 + x;
            if(src_y < BACK_ROWS)
            {
                vga::VGA_BUFFER[dst_idx] = back_buffer[src_y][x];
            }
            else
            {
                vga::VGA_BUFFER[dst_idx] = (static_cast<uint16_t>(vga::color) << 8) | ' ';
            }
        }
    }
}

void vga::vga_out_sc(char sc)
{
    uint16_t val = static_cast<uint16_t>(sc) | (static_cast<uint16_t>(vga::color) << 8);
    back_buffer[write_line][write_col] = val;
    write_col++;
}

void vga::put_char(char c)
{
    if(c == '\b')
    {
        if (write_col > 0)
        {
            write_col--;
        }
        else if (write_line > 0)
        {
            write_line--;
            write_col = 79;
        }
        back_buffer[write_line][write_col] = (static_cast<uint16_t>(vga::color) << 8) | ' ';
        goto refresh;
    }
    if(c == '\n')
    {
        write_col = 0;
        write_line++;
        if(write_line >= BACK_ROWS)
        {
            for(uint32_t y = 1; y < BACK_ROWS ; y++)
            {
                for(uint32_t x = 0; x < 80; x++)
                    back_buffer[y-1][x] = back_buffer[y][x];
            }
            write_line = BACK_ROWS - 1;
            for(uint32_t x = 0; x < 80; x++)
                back_buffer[write_line][x] = (static_cast<uint16_t>(vga::color) << 8) | ' ';
        }
        goto refresh;
    }
    vga::vga_out_sc(c);
    if (write_col >= 80)
    {
        write_col = 0;
        write_line++;
        if(write_line >= BACK_ROWS)
        {
            for(uint32_t y = 1; y < BACK_ROWS; y++)
            {
                for(uint32_t x = 0; x < 80; x++)
                    back_buffer[y-1][x] = back_buffer[y][x];
            }
            write_line = BACK_ROWS - 1;
            for(uint32_t x = 0; x < 80; x++)
                back_buffer[write_line][x] = (static_cast<uint16_t>(vga::color) << 8) | ' ';
        }
    }
refresh:
    uint32_t target_view = (write_line >= 24) ? (write_line - 24) : 0;
    if(view_offset < target_view)
    {
        view_offset = target_view;
    }
    vga_render();
    uint32_t disp_y = write_line - view_offset;
    vga::vga_move_cursor(write_col, disp_y);
}

void vga::vga_out_string(const char* st)
{
    while (*st != '\0')
    {
        vga::put_char(*st);
        st++;
    }
}

void vga::vga_clean()
{
    for(uint32_t y = 0; y < BACK_ROWS; y++)
    {
        for(uint32_t x = 0; x < 80; x++)
        {
            back_buffer[y][x] = (static_cast<uint16_t>(vga::color) << 8) | ' ';
        }
    }
    write_col = 0;
    write_line = 0;
    view_offset = 0;
    vga_render();
    vga::vga_move_cursor(0,0);
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
    int pos = y * 80 + x;
    io::write_uint8(0x3D4, 0x0F);
    io::write_uint8(0x3D5, (uint8_t)(pos & 0xFF));
    io::write_uint8(0x3D4, 0x0E);
    io::write_uint8(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void vga::vga_out_sixteen(uint32_t num)
{
    const char hex_table[] = "0123456789ABCDEF";
    for(int shift = 28; shift >= 0; shift -= 4)
    {
        uint8_t nibble = (num >> shift) & 0xF;
        char c = hex_table[nibble];
        vga::put_char(c);
    }
}

void vga::scroll_up(void)
{
    if(view_offset > 0)
    {
        view_offset--;
        vga_render();
    }
}

void vga::scroll_down(void)
{
    uint32_t max_view = (write_line >= 24) ? (write_line - 24) : 0;
    if(view_offset < max_view)
    {
        view_offset++;
        vga_render();
    }
}
