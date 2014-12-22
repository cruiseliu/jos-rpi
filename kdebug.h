#pragma once

#include <cstdint>

/**
 * @brief Get debug information of a program counter from stabs.
 *
 * Members are not accessible because currently we don't need so.
 *
 * This is a rewritten version of Eipdebuginfo. Although the original one is
 * almost usable out of box, our implementation is much simpler (well, at least
 * much shorter).
 *
 * The "binary search" of JOS Eipdebuginfo is O(n) in fact, I can't see the
 * benefit of using such a complicated but slow algorithm. So we just use
 * linear scanning instead, which makes it easy to detect file and function at
 * the same time.
 *
 * Note that due to the compiler setting our stabs are slightly different from
 * original JOS. SO entries of assembly files are "/tmp/xxxxxxxx.s", while SOL
 * entries are our expected file names. The address of boot.S in SO entry is
 * strangly 0x804c, so we must check its SOL entry manually.
 */

class DebugInfo {
public:
    /// Prepare the infomation, note that the parameter is pc rather than lr.
    DebugInfo(uintptr_t pc);

    /// Print all information to stdout.
    void show();

private:
    const char *file; ///< Source code file name for pc
    int line;         ///< Source code line number for pc
    const char *func; ///< Name of function containing pc, not null terminated
    int func_len;     ///< Length of function name
    uintptr_t addr;   ///< Offset from start of function

    bool valid;       ///< Whether or not the debug information is valid
    bool is_asm;      ///< Whether or not this is an assembly file
};
