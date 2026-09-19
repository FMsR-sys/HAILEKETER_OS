#ifndef VMM_HPP
#define VMM_HPP

#include "pmm.hpp"

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

using pde_t = uint32_t;  //页目录
using pte_t =uint32_t;   //页表

constexpr uint32_t PAGE_HAVE = 1 << 0;  //存在位，1=存在
constexpr uint32_t PAGE_RW = 1 << 1;    //读写位，1=可读可写，0=只读
constexpr uint32_t PAGE_USER = 1 << 2;  //权限位，1=Ring3，0=Ring0

constexpr uint32_t PAGE_HEAP = 1 << 9;   //标记内存堆HEAP

namespace vmm
{
    
    enum ERROR
    {
        SUCCESS = 0,   //无错误
        ERR_4K = 0x40,    //没有4K对齐
        ERR_PMM_ALLOCATE = 0x02,  //PMM物理页分配失败
        ERR_NO_INIT = 0x01,  //kernel_pd为空，VMM未初始化
        ERR_HAVE = 0xFF,    //目标已经存在映射（map 重复映射时报错）
        ERR_NOT_MAP = 0x03  //没有映射(get_b/unmap 查询不存在页面时报错)
    };

    extern ERROR err;

    extern pde_t* kernel_pd;    //虚拟地址指针

    void init();
    bool map(uint32_t a , uint32_t b , uint32_t flags);  //把a映射到b，a为虚拟地址，b为物理地址，需要4K对齐，成功返回true
    bool unmap(uint32_t a);  //取消a的映射，a为虚拟地址
    uint32_t get_b(uint32_t a);  //获得虚拟地址a的物理地址，无效返回0
    void cr3(uint32_t c);  //写入cr3寄存器，切换页目录
    void flush(uint32_t a);  //刷新a的TLB缓存，a为虚拟地址
}

#endif