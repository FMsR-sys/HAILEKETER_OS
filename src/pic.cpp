#include "pic.hpp"

namespace pic
{
    constexpr uint8_t MAIN_CMD    = 0x20;
    constexpr uint8_t MAIN_DATA   = 0x21;
    constexpr uint8_t SLAVE_CMD   = 0xA0;
    constexpr uint8_t SLAVE_DATA  = 0xA1;

    void remap (uint8_t main_off , uint8_t slave_off)
    {
        //初始化
        io::write_uint8 (MAIN_CMD , 0x11);
        io::wait();
        io::write_uint8 (SLAVE_CMD , 0x11);
        io::wait();

        //设置中断偏移
        io::write_uint8 (MAIN_DATA , main_off);
        io::wait();
        io::write_uint8 (SLAVE_DATA , slave_off);
        io::wait();
        
        //主从接线
        io::write_uint8 (MAIN_DATA , 0x04);
        io::wait();
        io::write_uint8 (SLAVE_DATA , 0x02);
        io::wait();

        //8086模式
        io::write_uint8 (MAIN_DATA , 0x01);
        io::wait();
        io::write_uint8 (SLAVE_DATA , 0x01);
        io::wait();

        io::write_uint8 (MAIN_DATA , 0xFF);
        io::write_uint8 (SLAVE_DATA , 0xFF);
    }

    void mask (uint8_t irq)
    {
        uint8_t port;

        if(irq < 8)
        {
            port = MAIN_DATA;
        }
        else
        {
            port = SLAVE_DATA;
            irq = irq - 8;
        }

        uint8_t old_value = io::read_uint8(port);
        old_value |= (1U << irq);
        io::write_uint8(port, old_value);
    }

    void unmask (uint8_t irq)
    {
        uint8_t port;

        if(irq < 8)
        {
            port = MAIN_DATA;
        }
        else
        {
            port = SLAVE_DATA;
            irq = irq - 8;
        }

        uint8_t old_value = io::read_uint8(port);
        old_value &= ~(1U << irq);
        io::write_uint8(port, old_value);
    }

    void eoi(uint8_t irq)
    {
        if (irq >= 8)
        {
            io::write_uint8 (SLAVE_CMD , 0x20);
        }
        io::write_uint8 (MAIN_CMD , 0x20);
    }
}