#pragma once

#include <cstdint>
#include <type_traits>

/**
 * @brief Address/pointer type.
 *
 * This class is designed for easy conversation between address and pointer
 * with zero overhead.
 *
 * Because we want a POD class, use initializer list (brace initializer)
 * instead of constructors. For example:
 * @code
 *     const Addr entry_addr = { 0x8000 };
 *     uint32_t mail = mmio_read({0x2000b880});
 * @endcode
 *
 * This class is highly under-development, only functions in use are provided.
 */
struct Addr {
    void operator-=(Addr a) { addr -= a.addr; }
    uint32_t operator[](int idx) { return *((uint32_t *)(addr) + idx); }
    explicit operator bool() { return addr; }

    uint32_t addr;
};

static inline bool operator< (Addr a1, Addr a2) { return a1.addr <  a2.addr; }
static inline bool operator> (Addr a1, Addr a2) { return a1.addr >  a2.addr; }
static inline bool operator<=(Addr a1, Addr a2) { return a1.addr <= a2.addr; }
static inline bool operator>=(Addr a1, Addr a2) { return a1.addr >= a2.addr; }
static inline bool operator==(Addr a1, Addr a2) { return a1.addr == a2.addr; }

static_assert(std::is_pod<Addr>::value, "Addr is not POD");
