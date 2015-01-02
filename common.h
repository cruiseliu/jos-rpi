#pragma once

typedef uintptr_t PAddr;
typedef uintptr_t VAddr;

typedef uint32_t Word;

static inline uint32_t round_down(int a, int n)
{
    return a & ~(n - 1);
}
