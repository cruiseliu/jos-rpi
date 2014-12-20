#include "kdebug.h"
#include "string.h"
#include "stdio.h"

/// Entries in the STABS table are formatted as follows.
struct Stab {
    uint32_t strx; ///< index into string table of name
    uint8_t type;  ///< type of symbol
    uint8_t other; ///< misc info (usually empty)
    uint16_t desc; ///< description field
    Addr value;    ///< value of symbol

    /// @name Type magic number
    //@{
    static const uint8_t FUN   = 0x24;
    static const uint8_t SLINE = 0x44;
    static const uint8_t SO    = 0x64;
    //@}
};

extern const Stab __stab_start[];    ///< Beginning of stabs table
extern const Stab __stab_end[];	     ///< End of stabs table
extern const char __stabstr_start[]; ///< Beginning of string table
extern const char __stabstr_end[];   ///< End of string table

DebugInfo::DebugInfo(uint32_t pc) : addr({pc}), valid(false)
{
    // The SO entry
    const Stab *so  = __stab_start + 1; // convenient for boot.S
    // The FUN entry
    const Stab *fun = nullptr;

    // Detect SO and FUN.
    const Stab *s = __stab_start;
    for (; s < __stab_end; ++s)
        if (s->type == Stab::SO || s->type == Stab::FUN) {
            if (s->value > addr)
                break;
            if (s->type == Stab::SO)
                so = s;
            else // s->type == Stab::FUN
                fun = s;
        }

    if (s == __stab_end) return;

    // SO entries of assembly files are temporary files, true ones are in SOL entries.
    // Assume an SO entry starts with '/' iff it is an assembly file ("/tmp/*.s").
    // For unknown reason the SO entry of boot.S is 0x804c rather than 0x8000,
    // this has been handled by the initial value of so. 
    if (__stabstr_start[so->strx] == '/') {
        // assert it matches "/tmp/*.s"
        ++so;
        // assert(so->type == Stab::SOL);

        fun = so;
        is_asm = true;
    } else {
        addr -= fun->value;
        is_asm = false;
    }

    // Detect line number.
    // We have iterated the SLINE entries already, so it's OK to do this one more time. 
    // As a debugging function, the performance is not critical.
    for (s = fun; s->type != Stab::SLINE && s < __stab_end; ++s) { }
    for (; s->type == Stab::SLINE && s->value <= addr && s < __stab_end; ++s) { }
    //assert(s[-1].type == Stab::SLINE && s[-1].value <= addr);

    if (s == __stab_end) return;

    if (is_asm)
        addr -= so->value;

    file = __stabstr_start + so->strx;
    line = (s - 1)->desc;
    func = __stabstr_start + fun->strx;
    func_len = strchrnul(func, ':') - func;

    valid = true;
}

void DebugInfo::show()
{
    if (valid)
        printf("    %s:%d: %.*s+%d\n", file, line, func_len, func, addr);
    else
        puts(CRED "    Invalid debug info" CEND);
}
