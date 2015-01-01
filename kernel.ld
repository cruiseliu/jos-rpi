ENTRY(_start)

MEMORY {
    boot : org = 0x8000, len = 1M
    kernel : org = 0xf0000000, len = 1M
}

SECTIONS {
    .init : {
        build/boot.o(.text* .data* .bss* .rodata*)
    } >boot

    __boot_end = ALIGN(4k);

    .text (__boot_end + 0xf0000000) : AT(__boot_end) {
        __text_start = ABSOLUTE(.);
        *(.text*)
        __text_end = ABSOLUTE(.);
    } >kernel

    .rodata : {
        __rodata_start = ABSOLUTE(.);
        *(.rodata*)
        __rodata_end = ABSOLUTE(.);
    } >kernel

    .stab : {
        __stab_start = ABSOLUTE(.);
        *(.stab);
        BYTE(0)
        __stab_end = ABSOLUTE(.);
    } >kernel

    .stabstr : {
        __stabstr_start = ABSOLUTE(.);
        *(.stabstr);
        BYTE(0)
        __stabstr_end = ABSOLUTE(.);
    } >kernel

    . = ALIGN(4k);

    .data : {
        __data_start = ABSOLUTE(.);
        *(.data)
        __data_end = ABSOLUTE(.);
    } >kernel

    .bss : {
        __bss_start = ABSOLUTE(.);
        *(.bss)
        __bss_end = ABSOLUTE(.);
    } >kernel
}
