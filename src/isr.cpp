#include "isr.hpp"
#include "pic.hpp"
#include "vga.hpp"
#include "io.hpp"
#include "shell.hpp"

// 用 switch 直接按 scancode 映射，彻底避免数组错位
static char scancode_to_char(uint8_t code, bool shift)
{
    switch (code)
    {
        // 数字行
        case 0x02: return shift ? '!' : '1';
        case 0x03: return shift ? '@' : '2';
        case 0x04: return shift ? '#' : '3';
        case 0x05: return shift ? '$' : '4';
        case 0x06: return shift ? '%' : '5';
        case 0x07: return shift ? '^' : '6';
        case 0x08: return shift ? '&' : '7';
        case 0x09: return shift ? '*' : '8';
        case 0x0A: return shift ? '(' : '9';
        case 0x0B: return shift ? ')' : '0';
        case 0x0C: return shift ? '_' : '-';
        case 0x0D: return shift ? '+' : '=';

        // 上排字母
        case 0x10: return shift ? 'Q' : 'q';
        case 0x11: return shift ? 'W' : 'w';
        case 0x12: return shift ? 'E' : 'e';
        case 0x13: return shift ? 'R' : 'r';
        case 0x14: return shift ? 'T' : 't';
        case 0x15: return shift ? 'Y' : 'y';
        case 0x16: return shift ? 'U' : 'u';
        case 0x17: return shift ? 'I' : 'i';
        case 0x18: return shift ? 'O' : 'o';
        case 0x19: return shift ? 'P' : 'p';
        case 0x1A: return shift ? '{' : '[';
        case 0x1B: return shift ? '}' : ']';

        // 中排字母
        case 0x1E: return shift ? 'A' : 'a';
        case 0x1F: return shift ? 'S' : 's';
        case 0x20: return shift ? 'D' : 'd';
        case 0x21: return shift ? 'F' : 'f';
        case 0x22: return shift ? 'G' : 'g';
        case 0x23: return shift ? 'H' : 'h';
        case 0x24: return shift ? 'J' : 'j';
        case 0x25: return shift ? 'K' : 'k';
        case 0x26: return shift ? 'L' : 'l';
        case 0x27: return shift ? ':' : ';';
        case 0x28: return shift ? '"' : '\'';

        // 下排字母
        case 0x29: return shift ? '~' : '`';
        case 0x2B: return shift ? '|' : '\\';
        case 0x2C: return shift ? 'Z' : 'z';
        case 0x2D: return shift ? 'X' : 'x';
        case 0x2E: return shift ? 'C' : 'c';
        case 0x2F: return shift ? 'V' : 'v';
        case 0x30: return shift ? 'B' : 'b';
        case 0x31: return shift ? 'N' : 'n';
        case 0x32: return shift ? 'M' : 'm';
        case 0x33: return shift ? '<' : ',';
        case 0x34: return shift ? '>' : '.';
        case 0x35: return shift ? '?' : '/';

        // 空格
        case 0x39: return ' ';

        default: return 0;
    }
}

static uint8_t last_scan = 0xFF;
static bool shift_left  = false;
static bool shift_right = false;
static bool ext_flag = false;

extern "C" void irq1_handler();
extern "C" void irq1_handler()
{
    uint8_t scancode = io::read_uint8(0x60);

    if(scancode == 0xE0)
    {
        ext_flag = true;
        pic::eoi(1);
        return;
    }

    bool released = (scancode & 0x80);
    uint8_t code = scancode & 0x7F;

    if(ext_flag)
    {
        ext_flag = false;
        if(!released)
        {
            if(code == 0x48) vga::scroll_up();
            if(code == 0x50) vga::scroll_down();
            if(code == 0x49) vga::scroll_up();
            if(code == 0x51) vga::scroll_down();

            if(code == 0x4B)
            {
                if(vga::cursor_x > 0) vga::cursor_x--;
                vga::vga_move_cursor(vga::cursor_x, vga::cursor_y);
            }

            if(code == 0x4D)
            {
                if(vga::cursor_x < vga::VGA_WIDE - 1) vga::cursor_x++;
                vga::vga_move_cursor(vga::cursor_x, vga::cursor_y);
            }
            pic::eoi(1);
            return;
        }
    }

    if(code == 0x2A)
    {
        shift_left = !released;
        pic::eoi(1);
        return;
    }

    if(code == 0x36)
    {
        shift_right = !released;
        pic::eoi(1);
        return;
    }

    if(released)
    {
        last_scan = 0xFF;
        pic::eoi(1);
        return;
    }

    if(scancode == last_scan)
    {
        pic::eoi(1);
        return;
    }

    last_scan = scancode;
    bool shift = shift_left || shift_right;

    if(code == 0x1C)
    {
        shell::enter();
        pic::eoi(1);
        return;
    }

    if(code == 0x0E)
    {
        shell::put_char('\b');
        pic::eoi(1);
        return;
    }

    char ch = scancode_to_char(code, shift);
    if(ch != 0)
    {
        shell::put_char(ch);
    }

    pic::eoi(1);
}