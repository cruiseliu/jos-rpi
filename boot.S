.section ".text.boot"

.globl _start

_start:
    mov sp, #0x8000
    mov fp, #0

    ldr r4, =__bss_start
    ldr r9, =__bss_end
    mov r5, #0
    mov r6, #0
    mov r7, #0
    mov r8, #0

@ clear bss
    b 2f
1:
    stmia r4!, {r5-r8}
2:
    cmp r4, r9
    blo 1b

@ set mmu entry
    ldr r4, mmu_data0
    ldr r9, mmu_addr0
    str r4, [r9]

    ldr r4, mmu_data1
    ldr r9, mmu_addr1
    str r4, [r9]

    ldr r4, mmu_data2
    ldr r9, mmu_addr2
    str r4, [r9]

    ldr r4, mmu_data3
    ldr r9, mmu_addr3
    str r4, [r9]

    ldr r4, mmu_data4
    ldr r9, mmu_addr4
    str r4, [r9]

    ldr r4, mmu_data5
    ldr r9, mmu_addr5
    str r4, [r9]

@ start mmu
    ldr r4, mmu_base
    ldr r9, mmu_flag
    mov r2,#0
    mcr p15,0,r2,c7,c7,0 ;@ invalidate caches
    mcr p15,0,r2,c8,c7,0 ;@ invalidate tlb

    mvn r2,#0
    mcr p15,0,r2,c3,c0,0 ;@ domain

    mcr p15,0,r4,c2,c0,0 ;@ tlb base
    mcr p15,0,r4,c2,c0,1 ;@ tlb base

    mrc p15,0,r2,c1,c0,0
    orr r2,r2,r9
    mcr p15,0,r2,c1,c0,0

@ load kernel_main
    ldr r3, =kernel_main
    blx r3

halt:
    wfe
    b halt

mmu_data0:
.word 0x0000000e
mmu_data1:
.word 0x20000002
mmu_data2:
.word 0x20100002
mmu_data3:
.word 0x20200002
mmu_data4:
.word 0x5c000002
mmu_data5:
.word 0x5c100002

mmu_addr0:
.word 0x00100000
mmu_addr1:
.word 0x00100800
mmu_addr2:
.word 0x00100804
mmu_addr3:
.word 0x00100808
mmu_addr4:
.word 0x00101700
mmu_addr5:
.word 0x00101704

mmu_base:
.word 0x00100000
mmu_flag:
.word 0x00800001
