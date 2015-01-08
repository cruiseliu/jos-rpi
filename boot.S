.globl _start

_start:
    @ Set up stack
    mov sp, #0x8000
    mov fp, #0

    @ Map first 1MB to it self
    mov r4, #0xe
    mov r5, #0
    str r4, [r5]

    @ Map first 1MB to high address (0xf0000000)
    mov r4, #0xe
    mov r5, #0x3c00
    str r4, [r5]

    @ Clear TLB
    mcr p15, 0, r4, c8, c7, 0

    @ Set to master of domain 0 (other domains not used)
    mov r4, #3
    mcr p15, 0, r4, c3, c0, 0

    @ Set (temporary) page table at bottom of stack
    mov r4, #0
    mcr p15, 0, r4, c2, c0, 2
    mcr p15, 0, r4, c2, c0, 0

    @ Turn on MMU
    mrc p15, 0, r4, c1, c0, 0
    orr r4, r4, #1            @ enable MMU
    orr r4, r4, #0x00800000   @ ARMv6 mode
    mcr p15, 0, r4, c1, c0, 0

    @ clear bss
    ldr r4, =__bss_start
    ldr r9, =__bss_end
    mov r5, #0
    mov r6, #0
    mov r7, #0
    mov r8, #0
    b 2f
1:
    stmia r4!, {r5-r8}
2:
    cmp r4, r9
    blo 1b

    @ call C++ code
    ldr r3, =kernel_main
    blx r3

halt:
    wfe
    b halt
