#pragma once

#include <cstdint>

//class Addr {
//public:
    //inline Addr() : m_addr(0) { }
    //inline Addr(uint32_t addr) : m_addr(addr) { }
    //inline Addr(void *addr) : m_addr((uint32_t) addr) { }
    //inline Addr(const Addr &addr) : m_addr(addr.m_addr) { }

    //inline Addr & operator=(uint32_t addr)
    //{
    //    m_addr = addr;
    //    return *this;
    //}

    //inline Addr & operator=(void *addr)
    //{
    //    m_addr = (uint32_t) addr;
    //    return *this;
    //}
    
    //inline uint32_t operator*() const
    //{
    //    return * (uint32_t *) m_addr;
    //}

    //inline Addr operator+(int offset) const
    //{
    //    return Addr(m_addr + offset);
    //}

    //inline Addr operator-(int offset) const
    //{
    //    return Addr(m_addr - offset);
    //}

    //uint32_t m_addr;
//};

typedef uint32_t Addr;
typedef uint32_t Bitset;

static inline void mmio_write(Addr addr, uint32_t data)
{
    asm volatile("str %[data], [%[addr]]" : : [addr]"r"(addr), [data]"r"(data));
}
 
static inline uint32_t mmio_read(Addr addr)
{
    uint32_t data;
    asm volatile("ldr %[data], [%[addr]]" : [data]"=r"(data) : [addr]"r"(addr));
    return data;
}
