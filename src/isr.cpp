#include "isr.hpp"
#include "pic.hpp"
#include "vga.hpp"
#include "io.hpp"

static const char sc_lower[] =
{
    0,   0,  '1','2','3','4','5','6','7','8','9','0','-','=', 0,
    0,  'q','w','e','r','t','y','u','i','o','p','[',']',  0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,
    '\\','z','x','c','v','b','n','m',',','.','/', 0,  '*',
    0,   ' ','0'
};
static const char sc_upper[] =
{
    0,   0,  '!','@','#','$','%','^','&','*','(',')','_','+', 0,
    0,  'Q','W','E','R','T','Y','U','I','O','P','{','}',  0,
    'A','S','D','F','G','H','J','K','L',':','"','~', 0,
    '|','Z','X','C','V','B','N','M','<','>','?', 0,  '*',
    0,   ' ','0'
};

static uint8_t last_scan = 0xFF;
static bool shift_left  = false;
static bool shift_right = false;
static bool ext_flag = false;   // 标记是否收到 0xE0 扩展前缀

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

    //=====方向键 扩展码=====
    if(ext_flag)
    {
        ext_flag = false;
        if(!released)
        {
            if(code == 0x48) // ↑
            {
                if(vga::cursor_y > 0) vga::cursor_y--;
                vga::vga_move_cursor(vga::cursor_x, vga::cursor_y);
                pic::eoi(1);
                return;
            }
            if(code == 0x50) // ↓
            {
                if(vga::cursor_y < vga::VGA_HIGH - 1) vga::cursor_y++;
                vga::vga_move_cursor(vga::cursor_x, vga::cursor_y);
                pic::eoi(1);
                return;
            }
            if(code == 0x4B) // ←
            {
                if(vga::cursor_x > 0) vga::cursor_x--;
                vga::vga_move_cursor(vga::cursor_x, vga::cursor_y);
                pic::eoi(1);
                return;
            }
            if(code == 0x4D) // →
            {
                if(vga::cursor_x < vga::VGA_WIDE - 1) vga::cursor_x++;
                vga::vga_move_cursor(vga::cursor_x, vga::cursor_y);
                pic::eoi(1);
                return;
            }
        }
    }

    // -------- Shift 左右按键 --------
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

    // 松开按键，清空防抖标记
    if(released)
    {
        last_scan = 0xFF;
        pic::eoi(1);
        return;
    }

    // 屏蔽硬件长按连发
    if(scancode == last_scan)
    {
        pic::eoi(1);
        return;
    }
    last_scan = scancode;

    bool shift = shift_left || shift_right;

    // 回车键 0x1C
    if(code == 0x1C)
    {
        vga::put_char('\n');
        pic::eoi(1);
        return;
    }
    // 退格键 0x0E
    if(code == 0x0E)
    {
        vga::put_char('\b');
        pic::eoi(1);
        return;
    }

    // 输出普通字符
    if(code < sizeof(sc_lower))
    {
        char ch;
        if(shift)
            ch = sc_upper[code];
        else
            ch = sc_lower[code];

        if(ch != 0)
        {
            vga::put_char(ch);
        }
    }

    pic::eoi(1);
}
