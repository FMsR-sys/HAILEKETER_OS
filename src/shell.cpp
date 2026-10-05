#include "shell.hpp"

namespace shell
{
    char input_buf[SHELL_BUF_LEN];  //输入缓冲区
    uint32_t input_pos = 0;         //字符写入位置下标

    static bool strcmp(const char* a , const char* b) //字符串比较
    {
        while (*a != 0 && *b !=0 && *a == *b)
        {
            a++;
            b++;
        }

        return (*a == 0 && *b == 0);
    }


    /*函数作用：按空格分割input_buf，生成argv参数数组
      argc：输出，参数总个数
      argv：输出，字符串指针数组，argv[0]是命令名，argv[1]第一个参数*/

    static void qiege(char** argv , int* argc)
    {
        *argc = 0;
        char* p = input_buf;

        while (*p != 0 && *argc < 15)   //最多15参数，argv[16]预留nullptr
        {
            while (*p == ' ' && *p != '\0')
            {
                p++;
            }

            if (*p == '\0')
            {
                break;
            }

            argv[(*argc)++] = p;

            while (*p != ' ' && *p != '\0')
            {
                p++;
            }

            if (*p != '\0')
            {
                *p = '\0';
                p++;
            }
        }
    }

    void init()
    {
        input_pos = 0;

        for (uint32_t i = 0;i <= SHELL_BUF_LEN;i++)
        {
            input_buf[i] = 0;   
        }

        vga::vga_out_string("> ");
    }

    void put_char(char a)
    {
        if (a == '\b')
        {
            if (input_pos > 0)
            {
                input_pos--;
                input_buf[input_pos] = '\0';

                vga::put_char('\b');
            }
            return;
        }

        if (input_pos >= SHELL_BUF_LEN - 1)
        {
            return;
        }

        input_buf[input_pos++] = a;
        input_buf[input_pos] = '\0';

        vga::put_char(a);
    }

    void enter()
    {
        vga::put_char('\n');
        char* argv[16] = {nullptr};
        int argc;

        qiege(argv , &argc);

        if (argc > 0)
        {
            vga::vga_out_string("Hello World\n");
            //暂停进度
        }

        input_pos = 0;
        init();
    }
}