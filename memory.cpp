#include "memory.h"
#include "memlayout.h"
#include "stdio.h"

extern char __end[];

namespace Memory {
    // FIXME: what's the exact alignment requirement? 1k or 4k?
    pte_t page_table[4096] __attribute__((aligned(0x1000))); // 4G / page_size

    struct PageInfo;

    extern PageInfo pages[];

    //PageInfo *pages = (PageInfo *)page_table_end;
    PageInfo *free_pages = nullptr;

    struct PageInfo {
        PageInfo *link;
        unsigned int ref;

        inline PAddr paddr() const { return (this - pages) * page_size; }
    };

    PageInfo pages[4096];

    const int npages = mem_size / page_size;

    uintptr_t end = (uintptr_t)__end;

    void init()
    {
        map(kern_vbase, 0, 0x0f000000);

        // Needed to change page table, I don't know why.
        map(0, 0);

        // Use translation table 0 for any virtual address
        asm volatile("mcr p15, 0, %[N], c2, c0, 2" : : [N]"r"(0));
        // Set translation table 0 base address
        asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : : [addr]"r"((uintptr_t)page_table - kern_vbase));
        // Invalidate TLB
        asm volatile("mcr p15, 0, %[data], c8, c7, 0" : : [data]"r"(0));
        // Change to domain client mode
        asm volatile("mcr p15, 0, %[data], c3, c0, 0" : : [data]"r"(1));

        // Doesn't need it any more.
        unmap(0);

        for (size_t i = npages - 1; i >= peri_pend / page_size; --i) {
            pages[i].link = free_pages;
            free_pages = &pages[i];
        }

        for (size_t i = peri_pbase / page_size - 1; i > 1; --i) {
            pages[i].link = free_pages;
            free_pages = &pages[i];
        }
    }

    void * boot_alloc(size_t size)
    {
        void *ret = (void *)end;
        end += round_up(size, 4); // align to word
        if (end >= 0xf0100000) {
            puts("boot_alloc: out of memory");
            return nullptr;
        }
        return ret;
    }

    PAddr page_alloc(VAddr va)
    {
        if (free_pages == nullptr)
            return 0; // 0 will never be dynamically allocated

        va = round_down(va, page_size);
        PageInfo *ret = free_pages;
        free_pages = free_pages->link;

        map(va, ret->paddr());

        return ret->paddr();
    }

    void map(VAddr va, PAddr pa, size_t size, pte_t perm, pte_t flags)
    {
        PAddr pend = pa + size;
        pa = round_down(pa, page_size);
        while (pa < pend) {
            page_table[page_num(va)] = PTE::gen(pa, perm, flags);
            pa += page_size;
            va += page_size;
        }
    }

    int unmap(VAddr va)
    {
        page_table[page_num(va)] = 0;
        //pte_t pte = get_pte(va);
        //if (pte == 0) return -1;

        //PageInfo *p = &pages[pte >> 20];
        //p->link = free_pages;
        //free_pages = p;

        //page_table[va >> 20] = 0;
        //return 0;
    }
}
