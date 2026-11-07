#ifndef VGA_HPP
#define VGA_HPP

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

namespace vga
{

    extern uint16_t* VGA_BUFFER;
    extern uint8_t cursor_x;
    extern uint8_t cursor_y;
    extern uint8_t color;

    const int VGA_HIGH = 25;
    const int VGA_WIDE = 80;

    // 历史缓冲行数（向上翻页用）
    const int HISTORY_ROWS = 512;

    void vga_out_string(const char* st);
    void vga_out_sc(char sc);
    uint8_t vga_color(uint8_t bg,uint8_t ps);       //背景bg,字体ps
    void vga_clean();
    void vga_set_color(uint8_t set);
    void vga_move_cursor(int x,int y);
    void vga_out_sixteen(uint32_t num);
    void put_char(char c);
    void scroll_up();
    void scroll_down();
    void vga_render();       // 重绘可见区（滚动/清屏后调用）
    void vga_scroll_to_bottom();  // 回到最新输出
}

#endif
