#pragma once

#include "common.h"
#include <cstddef>

const size_t mem_size = 0x10000000;

const uint32_t page_size = 0x00100000;

uint32_t * const page_table = (uint32_t *) 0xff300000;

const void * const page_table_end = page_table + 4096; // 4GB / page_size

typedef uintptr_t pte_t;

const uintptr_t peri_vbase = 0xff000000;
const uintptr_t peri_pbase = 0x20000000;

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

//extern uintptr_t *page_table;

const Word pte_mmio = 0;
const Word pte_normal = 0xc; // Cacheable and bufferable

// FIXME: move to something like mem.cpp
static inline void mem_map(VAddr va, PAddr pa, size_t size, Word flags = pte_normal)
{
    for (size_t i = 0; i < size; i += page_size) {
        page_table[(va + i) >> 20] = (pa + i) | flags | 2;
        //va += page_size;
        //pa += page_size;
    }
}
