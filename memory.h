#pragma once

#include "common.h"
#include "memlayout.h"
#include "mmu.h"
#include <cstddef>

namespace Memory {
    extern pte_t page_table[];

    void init();

    void * boot_alloc(size_t size);

    PAddr page_alloc(VAddr va);

    void map(VAddr va, PAddr pa, size_t size = 1,
            pte_t perm = PTE::krw, pte_t flags = PTE::normal);

    int unmap(VAddr va);

    inline pte_t get_pte(VAddr va) { return page_table[page_num(va)]; }
    inline pte_t get_pte(const void *va) { return get_pte((VAddr)va); }
}
