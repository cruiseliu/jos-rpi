#pragma once

#include "common.h"
#include "mmu.h"
#include <cstddef>

const size_t mem_size = 0x10000000;

const uintptr_t peri_vbase = 0xff000000;
const uintptr_t peri_pbase = 0x20000000;

const size_t peri_size = 0x00300000;

const uintptr_t peri_vend = 0xff300000;
const uintptr_t peri_pend = 0x20300000;

const uintptr_t framebuffer_vbase = 0xff400000;

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
