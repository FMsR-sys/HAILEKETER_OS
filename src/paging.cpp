#include "paging.hpp"

extern "C" void enable_paging(uint32_t pd_phys);
extern uint32_t __kernel_start;
extern uint32_t __kernel_end;

// 页目录放 BSS，4KB
static uint32_t g_page_dir[1024] __attribute__((aligned(4096)));

// 实际使用的页表（按需分配）
#define MAX_PAGETABLES 128
static uint32_t g_page_tables[MAX_PAGETABLES][1024] __attribute__((aligned(4096)));
static int g_pt_used = 0;

static uint32_t* get_or_create_pt(int pde)
{
    if (g_page_dir[pde] & 0x1)
    {
        return (uint32_t*)(g_page_dir[pde] & 0xFFFFF000);
    }
    if (g_pt_used >= MAX_PAGETABLES)
    {
        return nullptr;
    }
    uint32_t* pt = g_page_tables[g_pt_used++];
    for (int i = 0; i < 1024; i++)
    {
        pt[i] = 0;
    }
    g_page_dir[pde] = (uint32_t)pt | 0x3;
    return pt;
}

void identity_map_range(uint32_t start, uint32_t end)
{
    start &= 0xFFFFF000;
    end   = (end + 0xFFF) & 0xFFFFF000;

    for (uint32_t addr = start; addr < end; addr += 0x1000)
    {
        int pde = (addr >> 22) & 0x3FF;
        int pti = (addr >> 12) & 0x3FF;

        uint32_t* pt = get_or_create_pt(pde);
        if (pt == nullptr)
        {
            return;
        }
        pt[pti] = addr | 0x3;   // 恒等映射
    }
}

void identity_paging_init(uint32_t mem_end)
{
    // 清空页目录
    for (int pde = 0; pde < 1024; pde++)
    {
        g_page_dir[pde] = 0;
    }
    g_pt_used = 0;

    // 1) 低端 1MB（VGA、BIOS 数据区、E820 表）
    identity_map_range(0x00000000, 0x00100000);

    // 2) 内核镜像 + BSS
    identity_map_range((uint32_t)&__kernel_start, (uint32_t)&__kernel_end);

    // 3) 栈区
    identity_map_range(0x00800000, 0x00A00000);

    // 4) 可用物理内存全部恒等映射
    //    pmm 分配的是物理页，必须能直接访问到，否则一写就缺页
    if (mem_end > 0x00A00000)
    {
        identity_map_range(0x00A00000, mem_end);
    }

    enable_paging((uint32_t)g_page_dir);
}

uint32_t* identity_paging_dir()
{
    return g_page_dir;
}
