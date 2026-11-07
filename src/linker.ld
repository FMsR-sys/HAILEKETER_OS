ENTRY(kernel_main)

SECTIONS
{
    . = 0x100000;

    __kernel_start = .;

    .text ALIGN(4) :
    {
        *(.multiboot)
        *(.text*)
    }

    .rodata ALIGN(4) :
    {
        *(.rodata*)
    }

    .data ALIGN(4) :
    {
        *(.data*)
    }

    .bss ALIGN(4) (NOLOAD) :
    {
        __bss_start = .;
        *(COMMON)
        *(.bss*)
        __bss_end = .;
    }

    __kernel_end = .;

    /* 丢弃不需要的段，否则它们会挤进 .text 后面导致 objcopy 截断后面的段 */
    /DISCARD/ :
    {
        *(.eh_frame)
        *(.eh_frame_hdr)
        *(.comment)
        *(.note*)
    }
}
