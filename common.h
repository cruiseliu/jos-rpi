#pragma once

#include <cstdint>

// TODO: write a address/pointer class
typedef uint32_t Addr;

typedef uint32_t Bitset;

static inline void mmio_write(Addr addr, uint32_t data)
{
    asm volatile("str %[reg], [%[addr]]" : : [addr]"r"(addr), [reg]"r"(data));
}
 
static inline uint32_t mmio_read(Addr addr)
{
    uint32_t data;
    asm volatile("ldr %[reg], [%[addr]]" : [reg]"=r"(data) : [addr]"r"(addr));
    return data;
}
