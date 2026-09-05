#include "paging.hpp"
extern "C" void enable_paging(uint32_t pd_phys);

void identity_paging_init(uint32_t pd_phys)
{
    uint32_t* pd = reinterpret_cast<uint32_t*>(pd_phys);

    for(int pde = 0; pde < 1024; pde++)
    {
        pd[pde] = 0;
    }
    // pd_phys      = 页目录物理地址
    // pd_phys+0x1000 = 第一张页表的物理地址
    uint32_t pt_base_phys = pd_phys + 0x1000;

    // 循环填充全部 1024 个 PDE，覆盖完整4GB
    for(int pde = 0; pde < 1024; pde++)
    {
        uint32_t pt_phys = pt_base_phys + pde * 0x1000;
        uint32_t* pt = reinterpret_cast<uint32_t*>(pt_phys);

        // 填充这一张页表：4MB恒等映射
        for(int pte = 0; pte < 1024; pte++)
        {
            uint32_t phys = (pde * 0x400000U) + (pte * 0x1000U);
            pt[pte] = phys | 0x3;
        }

        pd[pde] = pt_phys | 0x3;
    }

    enable_paging(pd_phys);
}
