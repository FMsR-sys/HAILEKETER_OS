extern "C" void kernel_main()
{
    volatile unsigned short* vga = (volatile unsigned short*)0xB8000;
    const char* msg = "Hello World!";
    for (int i=0;msg[i]!='\0';i++)
    {
        vga[i] =(0x1F << 8) | msg[i];
    }
    while (true)
    {
        asm("hlt");
    }
}
