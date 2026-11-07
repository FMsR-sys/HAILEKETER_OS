#include "vga.hpp"
#include "io.hpp"

#define VGA_MEM 0xB8000

uint16_t* vga::VGA_BUFFER = reinterpret_cast<uint16_t*>(VGA_MEM);
uint8_t  vga::color    = 0x1E;
uint8_t  vga::cursor_x = 0;
uint8_t  vga::cursor_y = 0;

static uint16_t back_buffer[vga::HISTORY_ROWS][vga::VGA_WIDE];
static uint32_t write_line = 0;
static uint32_t view_top   = 0;

static inline uint16_t* mem() { return reinterpret_cast<uint16_t*>(VGA_MEM); }

static inline uint16_t make_cell(char c)
{
    return (static_cast<uint16_t>(vga::color) << 8) |
           static_cast<uint16_t>(static_cast<uint8_t>(c));
}

void vga::vga_render()
{
    uint16_t* base = mem();
    for (uint32_t y = 0; y < vga::VGA_HIGH; y++)
    {
        uint32_t src = view_top + y;
        for (uint32_t x = 0; x < vga::VGA_WIDE; x++)
        {
            base[y * vga::VGA_WIDE + x] =
                (src < vga::HISTORY_ROWS) ? back_buffer[src][x] : make_cell(' ');
        }
    }
}

static void render_current_line()
{
    if (write_line < view_top) return;
    uint32_t row = write_line - view_top;
    if (row >= vga::VGA_HIGH) return;

    uint16_t* base = mem() + row * vga::VGA_WIDE;
    for (uint32_t x = 0; x < vga::VGA_WIDE; x++)
    {
        base[x] = back_buffer[write_line][x];
    }
}

static void follow_cursor()
{
    if (write_line >= vga::VGA_HIGH)
    {
        view_top = write_line - (vga::VGA_HIGH - 1);
    }
    else
    {
        view_top = 0;
    }
    if (view_top + vga::VGA_HIGH > vga::HISTORY_ROWS)
    {
        view_top = vga::HISTORY_ROWS - vga::VGA_HIGH;
    }
}

static void scroll_buffer()
{
    for (uint32_t y = 1; y < vga::HISTORY_ROWS; y++)
    {
        for (uint32_t x = 0; x < vga::VGA_WIDE; x++)
        {
            back_buffer[y - 1][x] = back_buffer[y][x];
        }
    }
    uint32_t last = vga::HISTORY_ROWS - 1;
    for (uint32_t x = 0; x < vga::VGA_WIDE; x++)
    {
        back_buffer[last][x] = make_cell(' ');
    }
    write_line = last;
}

static void newline()
{
    vga::cursor_x = 0;
    write_line++;
    if (write_line >= vga::HISTORY_ROWS)
    {
        scroll_buffer();
    }
}

void vga::put_char(char c)
{
    uint32_t old_view = view_top;

    if (c == '\n')
    {
        newline();
    }
    else if (c == '\b')
    {
        if (vga::cursor_x > 0)
        {
            vga::cursor_x--;
        }
        else if (write_line > 0)
        {
            write_line--;
            vga::cursor_x = vga::VGA_WIDE - 1;
        }
        back_buffer[write_line][vga::cursor_x] = make_cell(' ');
    }
    else
    {
        back_buffer[write_line][vga::cursor_x] = make_cell(c);
        vga::cursor_x++;
        if (vga::cursor_x >= vga::VGA_WIDE)
        {
            newline();
        }
    }

    follow_cursor();

    if (view_top != old_view)
    {
        vga::vga_render();
    }
    else
    {
        render_current_line();
    }

    vga::cursor_y = static_cast<uint8_t>(write_line - view_top);
    vga::vga_move_cursor(vga::cursor_x, vga::cursor_y);
}

void vga::vga_out_sc(char sc) { put_char(sc); }

void vga::vga_out_string(const char* st)
{
    while (*st != '\0')
    {
        put_char(*st);
        st++;
    }
}

void vga::vga_clean()
{
    for (uint32_t y = 0; y < vga::HISTORY_ROWS; y++)
    {
        for (uint32_t x = 0; x < vga::VGA_WIDE; x++)
        {
            back_buffer[y][x] = make_cell(' ');
        }
    }
    write_line = 0;
    view_top   = 0;
    vga::cursor_x = 0;
    vga::cursor_y = 0;
    vga::vga_render();
    vga::vga_move_cursor(0, 0);
}

uint8_t vga::vga_color(uint8_t bg, uint8_t ps)
{
    uint8_t c = (bg << 4) | ps;
    vga::vga_set_color(c);
    return c;
}

void vga::vga_set_color(uint8_t set) { vga::color = set; }

void vga::vga_move_cursor(int x, int y)
{
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    vga::cursor_x = static_cast<uint8_t>(x);
    vga::cursor_y = static_cast<uint8_t>(y);

    int pos = y * vga::VGA_WIDE + x;
    io::write_uint8(0x3D4, 0x0F);
    io::write_uint8(0x3D5, static_cast<uint8_t>(pos & 0xFF));
    io::write_uint8(0x3D4, 0x0E);
    io::write_uint8(0x3D5, static_cast<uint8_t>((pos >> 8) & 0xFF));
}

void vga::vga_out_sixteen(uint32_t num)
{
    const char hex_table[] = "0123456789ABCDEF";
    for (int shift = 28; shift >= 0; shift -= 4)
    {
        uint8_t nibble = (num >> shift) & 0xF;
        vga::put_char(hex_table[nibble]);
    }
}

void vga::scroll_up()
{
    if (view_top > 0)
    {
        view_top--;
        vga::vga_render();
    }
}

void vga::scroll_down()
{
    uint32_t max_top = 0;
    if (write_line >= vga::VGA_HIGH)
    {
        max_top = write_line - (vga::VGA_HIGH - 1);
    }
    if (max_top + vga::VGA_HIGH > vga::HISTORY_ROWS)
    {
        max_top = vga::HISTORY_ROWS - vga::VGA_HIGH;
    }
    if (view_top < max_top)
    {
        view_top++;
        vga::vga_render();
    }
}

void vga::vga_scroll_to_bottom()
{
    follow_cursor();
    vga::vga_render();
}
