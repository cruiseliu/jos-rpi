/**
 * @file
 * @brief ARMv6 MMU definitions and functions.
 *
 * Specification of MMU is available in arm1176jzf-s.pdf,
 * chapter 6 (Memory Management Unit).
 */

#pragma once

typedef uint32_t pte_t;

/// The size of one "page" (section) : 1MB
const uint32_t page_size = 0x00100000;

/// log2(page_size)
const int page_shift = 20;

/// Get section base address of a virtual address.
static inline int page_num(VAddr va)
{
    return va >> page_shift;
}

/// The bits of page offset
const uint32_t page_offset_mask = page_size - 1;

/// Get page offset of an address.
static inline uintptr_t page_offset(uintptr_t addr)
{
    return addr & page_offset_mask;
}

namespace PTE {
    /// The magic number of "section" (1MB)
    /// bit18 = 0, bit1 = 1, bit0 = 0
    const pte_t section_magic = 0x00002;

    const pte_t magic = section_magic;

    const pte_t B    = 0x00004; ///< Bufferable
    const pte_t C    = 0x00008; ///< Cacheable
    const pte_t XN   = 0x00010; ///< Execute-Never
    //const pte_t P    = 0x00200; ///< Not supported by Raspberry Pi
    const pte_t AP0  = 0x00400; ///< Access permissions bit 0, always 1 in our case
    const pte_t AP1  = 0x00800; ///< Access permissions bit 1, 1 for user access
    const pte_t TEX  = 0x07000; ///< Type Extension Field
    const pte_t APX  = 0x08000; ///< Access permissions extension, 0 for write access
    const pte_t S    = 0x10000; ///< Shared
    const pte_t nG   = 0x20000; ///< Not-Global
    const pte_t NS   = 0x80000; ///< Non-Secure

    /// Read-Only for both kernel and user.
    const pte_t ro  = APX | AP1 | AP0;

    /// Read-Write for both kernel and user.
    const pte_t rw  = AP1 | AP0;

    /// Read-Only for kernel, no access for user.
    const pte_t kro = APX | AP0;

    /// Read-Write for kernel, no access for user.
    const pte_t krw = AP0;

    const pte_t normal = magic | B | C;
    const pte_t mmio   = magic; // TODO: use TEX bits

    inline pte_t gen(PAddr pa, pte_t perm = krw, pte_t flags = normal)
    {
        return (pa & ~page_offset_mask) | perm | flags;
    }
}
