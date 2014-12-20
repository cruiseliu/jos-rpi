#pragma once

#include <cstdint>

/**
 * @brief Address/pointer type.
 *
 * This class is designed for easy conversation between address and pointer
 * with zero overhead.
 *
 * Because the constructors are not inlined so well (especially for constants),
 * we will use initializer lists (brace initializer) instead. For example:
 * @code
 *     const Addr entry_addr = { 0x8000 };
 *     uint32_t mail = mmio_read({0x2000b880});
 * @endcode
 *
 * This class is highly under-development, only functions in use are provided.
 */
struct Addr {
    uint32_t m_addr;

    inline void operator-=(Addr a) { m_addr -= a.m_addr; }
    inline uint32_t operator[](int idx) { return *((uint32_t *)(m_addr) + idx); }
    explicit inline operator bool() { return m_addr; }
};

static inline bool operator< (Addr a1, Addr a2) { return a1.m_addr <  a2.m_addr; }
static inline bool operator> (Addr a1, Addr a2) { return a1.m_addr >  a2.m_addr; }
static inline bool operator<=(Addr a1, Addr a2) { return a1.m_addr <= a2.m_addr; }
static inline bool operator>=(Addr a1, Addr a2) { return a1.m_addr >= a2.m_addr; }
static inline bool operator==(Addr a1, Addr a2) { return a1.m_addr == a2.m_addr; }
