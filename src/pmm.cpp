#include "pmm.hpp"
#include "e820.hpp"


extern uint32_t __kernel_start;
extern uint32_t __kernel_end;


namespace pmm
{
    static uint8_t bitmap[ 1024*1024U / 8U ];

    void pmm_init()
    {
        for (uint32_t i = 0;i < sizeof(bitmap);i++)
        {
            bitmap[i] = 0xFF;
        }

        uint16_t count = *(reinterpret_cast<uint16_t*>(0x500));
        for (uint16_t i = 0;i < count;i++)
        {
            e820_record* rec = reinterpret_cast<e820_record*>(0x5000 + i*24);
            if (rec -> type == 1)
            {
                uint32_t start_addr = rec -> base_low;
                uint32_t area_len = rec -> len_low;
                uint32_t end_addr = start_addr + area_len;

                uint32_t start_page = start_addr / PAGE_SIZE;
                uint32_t end_page = end_addr / PAGE_SIZE;

                for (uint32_t page = start_page;page < end_page;page++)
                {
                    uint32_t byte_idx = page / 8;
                    uint8_t bit_idx = page % 8;
                    bitmap[byte_idx] &= ~(1U << bit_idx);
                }
            }
        }

        uint32_t kernel_start_addr = (uint32_t)&__kernel_start;
        uint32_t kernel_end_addr   = (uint32_t)&__kernel_end;

        kernel_end_addr = (kernel_end_addr + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

        uint32_t kernel_start_page = kernel_start_addr / PAGE_SIZE;
        uint32_t kernel_end_page   = kernel_end_addr / PAGE_SIZE;

        for(uint32_t page = kernel_start_page; page < kernel_end_page; page++)
        {
            uint32_t byte_idx = page / 8;
            uint32_t bit_idx  = page % 8;
            bitmap[byte_idx] |= (1U << bit_idx);
        }
        bitmap[256/8] &= ~(1U << (256%8));
    }

    uint32_t pmm_pages_allocate()
    {
        for (uint32_t byte = 0;byte < sizeof(bitmap);byte++)
        {
            if (bitmap[byte] == 0xFF)
            {
                continue;
            }
            
            for (uint32_t bit = 0;bit < 8;bit++)
            {
                uint32_t page = byte * 8 + bit;
                if (page < 256)
                {
                    continue;
                }
                if (!(bitmap[byte] & (1U << bit)))
                {
                    bitmap[byte] |= (1U << bit);
                    return page * PAGE_SIZE;
                }
            }
        }
        return 0x00000000;
    }

    void pmm_pages_release(uint32_t phys_addr)
    {
        uint32_t page = phys_addr / PAGE_SIZE;
        uint32_t byte_idx = page / 8;
        uint32_t bit_idx = page % 8;

        bitmap[byte_idx] &= ~(1U << bit_idx);
    }

    void pmm_mark_used(uint32_t phys_addr)
    {
        uint32_t page = phys_addr / PAGE_SIZE;
        uint32_t byte_idx = page / 8;
        uint32_t bit_idx = page % 8;

        bitmap[byte_idx] |= (1U << bit_idx);
    }

    void pmm_mark_free(uint32_t phys_addr)
    {
        uint32_t page = phys_addr / PAGE_SIZE;
        uint32_t byte_idx = page / 8;
        uint32_t bit_idx = page % 8;

        bitmap[byte_idx] &= ~(1U << bit_idx);
    }

    bool pmm_is_free(uint32_t phys_addr)
    {
        uint32_t page = phys_addr / PAGE_SIZE;
        uint32_t byte_idx = page / 8;
        uint32_t bit_idx = page % 8;

        return (bitmap[byte_idx] & (1U << bit_idx)) != 0;
    }

    bool pmm_is_used(uint32_t phys_addr)
    {
        return !pmm_is_free(phys_addr);
    }

}
