ENTRY(_start)

SECTIONS
{
    . = 0x8000;
    __start = .;

    __text_start = .;
    .text :
    {
        KEEP(*(.text.boot))
        *(.text)
    }
    . = ALIGN(4096);
    __text_end = .;

    __rodata_start = .;
    .rodata :
    {
        *(.rodata)
    }
    . = ALIGN(4096);
    __rodata_end = .;

    __stab_start = .;
    .stab :
    {
        *(.stab);
        BYTE(0)
    }
    __stab_end = .;

    __stabstr_start = .;
    .stabstr :
    {
        *(.stabstr);
        BYTE(0)
    }
    __stabstr_end = .;

    . = ALIGN(4096);

    __data_start = .;
    .data :
    {
        *(.data)
    }
    . = ALIGN(4096);
    __data_end = .;

    __bss_start = .;
    .bss :
    {
        bss = .;
        *(.bss)
    }
    . = ALIGN(4096);
    __bss_end = .;

    __end = .;
}
