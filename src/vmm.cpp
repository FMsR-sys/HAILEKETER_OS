#include "vmm.hpp"
#include "pmm.hpp"
#include "kmalloc.hpp"
#include "vga.hpp"
#include "paging.hpp"

//阿巴阿巴 ^q^

extern uint32_t __kernel_start;
extern uint32_t __kernel_end;

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
{    ERROR err = SUCCESS;
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

        // 复用 identity_paging_init() 建好的页目录，不新建
        // （新建会让那部分恒等映射丢失，写页表时立刻缺页）
        kernel_pd = reinterpret_cast<pde_t*>(identity_paging_dir());

        if (kernel_pd == nullptr)
        {
            err = ERR_NO_INIT;
            return;
        }

        // 其余区域交给缺页按需补，这里不再预先映射
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

        uint32_t count = 0;
        
        for (int i = 0;i < 1024;i++)
        {
            if (pt[i] & PAGE_HAVE)
            {
                count++;
                break;
            }
        }

        if (count == 0)
        {
            uint32_t pt_phys = pde_entry & 0xFFFFF000;
            pmm::pages_release(pt_phys);
            kernel_pd[pdi] = 0;
        }

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

    // 缺页处理：按需分配物理页并建立恒等映射
    // 返回 true 表示已修复，false 表示无法处理（真错误）
    bool handle_fault(uint32_t fault_addr, uint32_t error_code)
    {
        // 地址需要 4K 对齐
        uint32_t page_addr = fault_addr & 0xFFFFF000;

        // 如果这个虚拟地址已经有映射（比如写保护），不能自动修复
        if (kernel_pd == nullptr)
        {
            return false;
        }

        uint32_t pdi = (page_addr >> 22) & 0x3FF;
        uint32_t pti = (page_addr >> 12) & 0x3FF;

        pde_t pde_entry = kernel_pd[pdi];
        if (pde_entry & PAGE_HAVE)
        {
            pte_t* pt = reinterpret_cast<pte_t*>(pde_entry & 0xFFFFF000);
            if (pt[pti] & PAGE_HAVE)
            {
                // 已映射却触发缺页，多半是权限问题，不自动处理
                return false;
            }
        }

        uint32_t phys = pmm::pages_allocate();
        if (phys == 0)
        {
            return false;   // 内存耗尽
        }

        // 清空新页，避免把旧数据读出去
        memset(reinterpret_cast<void*>(phys), 0, 0x1000);

        if (!map(page_addr, phys, PAGE_HAVE | PAGE_RW))
        {
            pmm::pages_release(phys);
            return false;
        }

        return true;
    }

}

// 页错误中断入口 (由 idt.asm 的 isr14_stub 调用)
extern "C" void page_fault_handler(uint32_t error_code)
{
    uint32_t fault_addr;
    asm volatile("mov %%cr2, %0" : "=r"(fault_addr));

    if (vmm::handle_fault(fault_addr, error_code))
    {
        return;     // 已按需补上，iret 回去继续执行
    }

    // 真正的错误：打印信息后挂起
    vga::vga_out_string("\n[PAGE FAULT] unhandled\n  CR2 = ");
    vga::vga_out_sixteen(fault_addr);
    vga::vga_out_string("\n  ERR = ");
    vga::vga_out_sixteen(error_code);
    vga::vga_out_string("\n  P = ");
    vga::vga_out_sixteen((error_code >> 0) & 1);
    vga::vga_out_string(" W = ");
    vga::vga_out_sixteen((error_code >> 1) & 1);
    vga::vga_out_string(" U = ");
    vga::vga_out_sixteen((error_code >> 2) & 1);
    vga::vga_out_string("\n");

    for (;;)
    {
        asm volatile("cli; hlt");
    }
}