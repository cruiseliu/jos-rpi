#pragma once

#include <cstdint>

typedef uintptr_t PAddr;
typedef uintptr_t VAddr;

typedef uintptr_t paddr_t;
typedef uintptr_t vaddr_t;

typedef uint32_t Word;

static inline uint32_t round_down(int a, int n)
{
    return a & ~(n - 1);
}

static inline uint32_t round_up(int a, int n)
{
    return round_down(a + n - 1, n);
}
