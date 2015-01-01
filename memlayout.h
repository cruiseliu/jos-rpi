#pragma once

const uintptr_t peri_vbase = 0xff000000;
const uintptr_t peri_pbase = 0x20000000;

const uintptr_t kern_vbase = 0xf0000000;

constexpr uintptr_t peri_vaddr(uintptr_t paddr)
{
    //return paddr;
    return paddr - peri_pbase + peri_vbase;
}

static inline uintptr_t paddr(void *vaddr)
{
    //return (uintptr_t) vaddr;
    return (uintptr_t) vaddr - kern_vbase;
}
