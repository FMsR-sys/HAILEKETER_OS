ENTRY(_start)
SECTIONS
{
    . = 0x100000;
    .text ALIGN(4K) :
    {
        *(.boot)
        *(.text)
    }
}
