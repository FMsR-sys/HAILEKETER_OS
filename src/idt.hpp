#ifndef IDT_HPP
#define IDT_HPP

//  awa
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

struct idt_entry
{
    uint16_t offest_low; //低十六位
    uint16_t selector;   //段选择子
    uint8_t zero;        //保留字段，填0
    uint8_t flags;        //标志位，门类型+权限
    uint16_t offest_high;//高十六位
}__attribute__((packed));

struct idt_ptr
{
    uint16_t limit;   //总字节数−1
    uint32_t base;    //物理起始地址
}__attribute__((packed));


namespace idt
{
    void init();
    void set(uint8_t index , uint32_t offest , uint16_t sel , uint8_t flags);  //中断号，中断处理函数的内存地址，段选择子，门的属性标志字节
}

#endif