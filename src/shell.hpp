#ifndef SHELL_HPP
#define SHELL_HPP

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

namespace shell
{
    constexpr uint32_t SHELL_BUF_LEN = 512;
    extern char input_buf[SHELL_BUF_LEN];
    extern uint32_t input_pos;

    void init();
    void put_char(char a);
    void enter();
    //void commend(const char* cmd);
}

#endif