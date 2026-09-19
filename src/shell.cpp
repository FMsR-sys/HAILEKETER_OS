#include "shell.hpp"
#include "vga.hpp"

namespace shell
{
    char input_buf[SHELL_BUF_LEN];
    uint32_t input_pos = 0;

    void init()
    {
        input_pos = 0;
        uint32_t b = 0;

        for(uint32_t i = 0; i < SHELL_BUF_LEN; i++)
        {
            input_buf[i] = 0;
        }

        vga::vga_out_string("\n> ");
    }

    void put_char(char a)
    {
        if (a == '\b')
        {
            if (input_pos > 0)
            {
                input_pos--;
                input_buf[input_pos] = 0;
                vga::put_char('\b');
            }

            return;
        }

        if (input_pos >= SHELL_BUF_LEN - 1)
        {
            return;
        }

        input_buf[input_pos++] = a;
        input_buf[input_pos] = 0;
        vga::put_char(a);
    }

    void enter()
    {
        vga::put_char('\n');
        vga::vga_out_string(input_buf);
        vga::put_char('\n');

        input_pos = 0;
        for(uint32_t i = 0; i < SHELL_BUF_LEN; i++)
        {
            input_buf[i] = 0;
        }
        vga::vga_out_string("> ");
    }
}