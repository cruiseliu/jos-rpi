.globl _start

_start:
    mov sp, #0x8000
    mov fp, #0

    ldr r4, mmu_data0
    ldr r9, mmu_addr0
    str r4, [r9]

    ldr r4, mmu_data1
    ldr r9, mmu_addr1
    str r4, [r9]

    ldr r4, mmu_data13
    ldr r9, mmu_addr13
    str r4, [r9]

    ldr r4, mmu_base
    ldr r9, mmu_flag
    mov r2, #0

    mcr p15, 0, r2, c7, c7, 0
    mcr p15, 0, r2, c8, c7, 0

    mvn r2, #0
    mcr p15, 0, r2, c3, c0, 0

    mcr p15, 0, r4, c2, c0, 0
    mcr p15, 0, r4, c2, c0, 1

    mrc p15, 0, r2, c1, c0, 0
    orr r2, r2, r9
    mcr p15, 0, r2, c1, c0, 0

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

@ load kernel_main
    ldr r3, =kernel_main
    blx r3

halt:
    wfe
    b halt

mmu_data0: @ low address
.word 0x0000000e
mmu_addr0:
.word 0x00100000

mmu_data1: @ high address
.word 0x0000000e
mmu_addr1:
.word 0x00103c00

mmu_data13: @ low address
.word 0x0010000e
mmu_addr13:
.word 0x00103fcc

mmu_base:
.word 0x00100000

mmu_flag:
.word 0x00800001
