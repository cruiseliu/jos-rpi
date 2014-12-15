#pragma once

static inline void mmio_write(uint32_t addr, uint32_t data)
{
    asm volatile("str %[data], [%[addr]]" : : [addr]"r"(addr), [data]"r"(data));
}
 
static inline uint32_t mmio_read(uint32_t addr)
{
    uint32_t data;
    asm volatile("ldr %[data], [%[addr]]" : [data]"=r"(data) : [addr]"r"(addr));
    return data;
}
