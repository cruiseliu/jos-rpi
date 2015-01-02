#pragma once

#include "common.h"

const uint32_t page_size = 0x00100000;

const uintptr_t peri_vbase = 0xff000000;
const uintptr_t peri_pbase = 0x20000000;

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

extern uintptr_t *page_table;

// FIXME: move to something like mem.cpp

static inline void mem_map(VAddr va, PAddr pa, PAddr pa_end)
{
    while (pa < pa_end) {
        page_table[va >> 20] = pa | 2;
        va += page_size;
        pa += page_size;
    }
}
