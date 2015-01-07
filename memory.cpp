#include "memory.h"
#include "memlayout.h"
#include "stdio.h"

extern char __end[];

namespace Memory {
    struct PageInfo;

    PageInfo *pages = (PageInfo *)page_table_end;
    PageInfo *free_pages = nullptr;

    struct PageInfo {
        PageInfo *link;
        unsigned int ref;

        inline PAddr paddr() const { return (this - pages) * page_size; }
    };

    const int npages = mem_size / page_size;

    uintptr_t end = (uintptr_t)__end;

    void init()
    {
        page_table[0xff0] = gen_pte(0x20000000, 0);
        page_table[0xff1] = gen_pte(0x20100000, 0);
        page_table[0xff2] = gen_pte(0x20200002, 0);

        mem_map(kern_vbase, 0, 0x0f000000);

        for (int i = npages - 1; i >= peri_pend / page_size; --i) {
            pages[i].link = free_pages;
            free_pages = &pages[i];
        }

        for (int i = peri_pbase / page_size - 1; i > 1; --i) {
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

        va = round_up(va, page_size);
        PageInfo *ret = free_pages;
        free_pages = free_pages->link;

        map_addr(va, ret->paddr());

        return ret->paddr();
    }

    void map_addr(VAddr va, PAddr pa, Word perm)
    {
        page_table[va >> 20] = pa | perm | 2;
    }

    int unmap(VAddr va)
    {
        pte_t pte = get_pte(va);
        if (pte == 0) return -1;

        PageInfo *p = &pages[pte >> 20];
        p->link = free_pages;
        free_pages = p;

        page_table[va >> 20] = 0;
        return 0;
    }
}
