#ifndef SHELL_HPP
#define SHELL_HPP

#include "vga.hpp"

#define SHELL_BUF_LEN 512

namespace shell
{
    extern char input_buf [SHELL_BUF_LEN];
    extern uint32_t input_pos;

    void init();  //初始化
    void put_char(char c);
    void enter();

    static void qiege(char** argv , int* argc);  //切割数/字符串数组指针
    static bool strcmp(const char* a , const char* b);  //自定义长度字符串比较
}

#endif