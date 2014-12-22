#pragma once

#include <cstdint>

/**
 * @file
 * ARM instructions.
 */

/// Store data to addr (str), this name is for readability.
static inline void mmio_write(uintptr_t addr, uint32_t data)
{
    asm volatile("str %[reg], [%[addr]]" : : [addr]"r"(addr), [reg]"r"(data));
}
 
/// Load from addr (ldr), this name is for readability.
static inline uint32_t mmio_read(uintptr_t addr)
{
    uint32_t data;
    asm volatile("ldr %[reg], [%[addr]]" : [reg]"=r"(data) : [addr]"r"(addr));
    return data;
}
