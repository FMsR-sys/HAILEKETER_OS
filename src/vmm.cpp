#include "vmm.hpp"
#include "pmm.hpp"
#include "kmalloc.hpp"

//阿巴阿巴 ^q^

void* memset(void* dst, uint8_t val, uint32_t len)  //dst起始地址，val填充值，len字节
{
    uint8_t* p = reinterpret_cast<uint8_t*>(dst);
    for(uint32_t i = 0; i < len; i++)
    {
        p[i] = val;
    }
    return dst;
}

namespace vmm
{
    ERROR err = SUCCESS;
    pde_t* kernel_pd;

    void cr3(uint32_t c)
    {
        asm volatile("mov %%eax , %%cr3" : : "a"(c));
    }

    void flush(uint32_t a)
    {
        asm volatile("invlpg (%0)" : : "r"(a) : "memory");
    }

    bool map(uint32_t a , uint32_t b , uint32_t flags)
    {
        err = SUCCESS;

        if (kernel_pd == nullptr)
        {
            err = ERR_NO_INIT;
            return false;
        }

        if ((a & 0xFFF) != 0 || (b & 0xFFF) != 0)
        {
            err = ERR_4K;
            return false;
        }

        uint32_t pdi = (a >> 22) & 0x3FF;   //获得页目录索引PDI
        uint32_t pti = (a >> 12) & 0x3FF;   //获得页表索引PTI

        pde_t & pde = kernel_pd[pdi];

        if (! (pde & PAGE_HAVE))
        {
            uint32_t pt_phys = pmm::pages_allocate();

            if (pt_phys == 0)
            {
                err = ERR_PMM_ALLOCATE;
                return false;
            }

            //清零页表
            memset(reinterpret_cast<void*>(pt_phys),0,0x1000);
            pde = pt_phys | PAGE_HAVE | PAGE_RW;

        }

        pte_t* pt = reinterpret_cast<pte_t*>(pde & 0xFFFFF000);

        if (pt[pti] & PAGE_HAVE)  //保护用，防止覆盖
        {
            err = ERR_HAVE;
            return false;
        }

        pt[pti] = (b & 0xFFFFF000) | flags;

        flush(a);
        return true;
    }

    void init()
    {
        err = SUCCESS;

        uint32_t pd_phys = pmm::pages_allocate();

        if (pd_phys == 0)
        {
            err = ERR_PMM_ALLOCATE;
            return;
        }

        memset(reinterpret_cast<void*>(pd_phys), 0, 0x1000);

        kernel_pd = reinterpret_cast<pde_t*>(pd_phys);

        //内核基础部分
        for (uint32_t virt = 0;virt < 0x1000000;virt += 0x1000)
        {
            map(virt,virt,PAGE_HAVE | PAGE_RW);
        }

        //堆区域
        uint32_t heap_start = heap::HEAP_START;
        uint32_t heap_size = heap::HEAP_SIZE;
        uint32_t heap_end = heap_start + heap_size;

        if (heap_end % 0x1000 != 0)  //4K补全兜底
        {
            heap_end += (0x1000 - (heap_end % 0x1000));
        }

        for (uint32_t virt = heap_start;virt < heap_end;virt += 0x1000)
        {
            map(virt,virt,PAGE_HAVE | PAGE_RW | PAGE_HEAP);
        }

        cr3(pd_phys);  //加载页目录
    }

    bool unmap(uint32_t a)
    {
        err = SUCCESS;
        if ((a & 0xFFF) != 0)
        {
            err = ERR_4K;
            return false;
        }

        uint32_t virt = a & 0xFFFFF000;

        if(kernel_pd == nullptr)
        {
            err = ERR_NO_INIT;
            return false;
        }

        uint32_t pdi = (virt >> 22) & 0x3FF;   //获得页目录索引PDI
        uint32_t pti = (virt >> 12) & 0x3FF;   //获得页表索引PTI

        pde_t pde_entry = kernel_pd[pdi];   //读页目录项PDE

        if (!(pde_entry & PAGE_HAVE))
        {
            err = ERR_HAVE;
            return false;
        }

        pte_t* pt = reinterpret_cast<pte_t*>(pde_entry & 0xFFFFF000);  //拿页表PT

        pte_t pte_entry = pt[pti];

        if (!(pte_entry & PAGE_HAVE))
        {
            err = ERR_HAVE;
            return false;
        }

        //释放物理页
        uint32_t phys_page = pte_entry & 0xFFFFF000;
        pmm::pages_release(phys_page);

        //清零
        pt[pti] = 0;

        flush(virt);
        return true;
    }

    uint32_t get_b(uint32_t a)
    {
        err = SUCCESS;

        //检查4K对齐
        if ((a & 0xFFF) != 0)
        {
            err = ERR_4K;
            return 0;
        }

        //不是，都写多少个4K检查和判断初始化了
        if (kernel_pd == nullptr)
        {
            err = ERR_NO_INIT;
            return 0;
        }

        uint32_t pdi = (a >> 22) & 0x3FF;
        uint32_t pti = (a >> 12) & 0x3FF;

        pde_t pde_entry = kernel_pd[pdi];

        if (!(pde_entry & PAGE_HAVE))  //又是这玩意，我都快背过了
        {
            err = ERR_NOT_MAP;
            return 0;
        }

        pte_t* pt = reinterpret_cast<pte_t*>(pde_entry & 0xFFFFF000);

        pte_t pte_entry = pt[pti];
        if (!(pte_entry & PAGE_HAVE))
        {
            err = ERR_NOT_MAP;
            return 0;
        }

        uint32_t d = pte_entry & 0xFFFFF000;
        return d;
    }
}