#pragma once

#include "common.h"
#include "memlayout.h"
#include <cstddef>

namespace Memory {
    inline pte_t gen_pte(PAddr pa, Word flags = 0) { return pa | flags | 2; }
    inline int pte_index(VAddr va) { return va >> 20; }
    inline Word pte_flags(pte_t pte) { return pte & 0x000fffff; }

    void init();
    void * boot_alloc(size_t size);
    PAddr page_alloc(VAddr va);
    void map_addr(VAddr va, PAddr pa, Word perm = pte_normal);
    int unmap(VAddr va);

    inline pte_t get_pte(VAddr va) { return page_table[pte_index(va)]; }
    inline pte_t get_pte(const void *va) { return get_pte((VAddr)va); }
}
