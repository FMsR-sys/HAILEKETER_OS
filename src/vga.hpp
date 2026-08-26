#ifndef VGA_HPP
#define VGA_HPP

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;


extern uint16_t* VGA_BUFFER;
extern const uint8_t OUT_COLOR;
extern uint8_t cursor_x;
extern uint8_t cursor_y;

const int VGA_HIGH = 25;
const int VGA_WIDE = 80;

void vga_out_string(int x,int y,const char* st);
void vga_out_sc(int x, int y,char sc);

#endif
