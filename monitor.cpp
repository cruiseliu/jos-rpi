#include "monitor.h"
#include "kdebug.h"
#include "addr.h"
#include "stdio.h"
#include "string.h"
#include "memory.h"
#include "stdlib.h"
#include <cstdint>

namespace Monitor {
    /// Information of a shell command
    struct Command {
        /// The command name
        const char *name;
        /// The command description
        const char *desc;
        /// Pointer to the function of command
        int (*func)(int argc, char *argv[]);
    };

    /// Command list
    const Command commands[] = {
        { "help",      "Display this list of commands",             help },
        { "kerninfo",  "Display information about the kernel",      kerninfo },
        { "backtrace", "Display stack backtrace",                   backtrace },
        { "showmap",   "Display virtual memory mappings",           showmap },
        { "alloc",     "Alloc a page",                              alloc },
        { "write",     "Write characters to memory",                write },
        { "read",      "Read characters from memory",               read },
        { "map",       "Map a virtual address to physical address", map },
        { "unmap",     "Unmap a virtual addres",                    unmap }
    };

    /// Number of available commands
    const int commands_num = sizeof(commands) / sizeof(Command);

    /* Kernel Shell Utilities */

    int help(int argc, char *argv[])
    {
        for (int i = 0; i < commands_num; ++i)
            printf("%s - %s\n", commands[i].name, commands[i].desc);
        return 0;
    }

    extern "C" {
        //extern const char _start[], __start[], __end[];
        extern const char __end[];
        extern const char __text_start[], __rodata_start[], __data_start[], __bss_start[];
    }

    int kerninfo(int argc, char *argv[])
    {
        puts  ("Special kernel symbols:");
        //printf("  _start %08x\n", _start);
        printf("  text   %08x\n", __text_start);
        printf("  rodata %08x\n", __rodata_start);
        printf("  data   %08x\n", __data_start);
        printf("  bss    %08x\n", __bss_start);
        printf("  end    %08x\n", __end);
        // It's not rounded up, this minus error should be acceptable.
        //printf("Kernel executable memory footprint: %dKB\n", (__end - __start) / 1024);
        return 0;
    }

    /**
     * Documentations are not always reliable. The official website[1] said
     * stack frame layout below fp should be (pc,lr,sp,fp). However I
     * disassambled my kernel and found the first instruction of each function
     * is `push {fp, lr}`, which means fp[0] is lr and fp[-1] is old fp.
     *
     * The arguments are not on the stack thus invisible to backtrace.
     *
     * [1] http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0041c/Cegbajdj.html
     */
    int backtrace(int argc, char *argv[])
    {
        // A new way to get register value
        register uintptr_t reg_fp asm ("fp");
        Addr fp {reg_fp};

        for (; fp; fp = {fp[-1]}) {
            printf("fp %08x  lr %08x\n", fp[-1], fp[0]);
            DebugInfo(fp[0] - 4).show();
        }

        return 0;
    }

    static inline void print_seg(VAddr start, VAddr end, pte_t pte)
    {
        printf("%08x~%08x %03x %05x\n", start, end - 1, pte >> 20, pte & 0xfffff);
    }

    int showmap(int argc, char *argv[])
    {
        VAddr va = 0;
        VAddr start_va = va;
        pte_t prev_pte = Memory::get_pte(va);

        for (va += page_size; va != 0; va += page_size) {
            pte_t cur_pte = Memory::get_pte(va);
            if (!((cur_pte == 0 && prev_pte == 0) || cur_pte == prev_pte + page_size)) {
                if (prev_pte != 0)
                    print_seg(start_va, va, prev_pte - (va - start_va - page_size));
                start_va = va;
            }
            prev_pte = cur_pte;
        }

        if (prev_pte != 0)
            print_seg(start_va, va, prev_pte - (va - start_va - page_size));

        return 0;
    }

    int alloc(int argc, char *argv[])
    {
        if (argc != 2) {
            puts("Usage: alloc SIZE");
            return 1;
        }

        void *addr = Memory::boot_alloc(atoi(argv[1]));

        if (addr != nullptr)
            printf("Allocated at virtual address %08x\n", addr);
        else {
            puts("Failed");
            return 2;
        }
        return 0;
    }

    int write(int argc, char *argv[]) {
        if (argc < 3) {
            puts("Usage: write ADDRESS WORDS");
            return 1;
        }

        char *addr = (char *)atoi(argv[1], 16);

        if ((uintptr_t)addr < kern_vbase || (uintptr_t)addr >= peri_vbase) {
            printf("Writing to virtual address %08x, are you sure? (y/N) ", addr);
            char ch = getchar();
            putchar('\n');
            if (ch != 'y') return 0;
        }

        for (int i = 2; i < argc; ++i) {
            for (char *ch = argv[i]; *ch; ++ch)
                *(addr++) = *ch;
            *(addr++) = ' ';
        }
        *addr = '\0';

        return 0;
    }

    int read(int argc, char *argv[])
    {
        if (argc < 2) {
            puts("Usage: read [-x] ADDRESS [SIZE]");
            return 1;
        }

        bool hex_mode = argv[1][0] == '-' && argv[1][1] == 'x';

        char *addr = (char *)atoi(hex_mode ? argv[2] : argv[1], 16);

        int size = 16;
        if (argc > (hex_mode ? 3 : 2))
            size = atoi(hex_mode ? argv[3] : argv[2]);

        if (!hex_mode)
            for (int i = 0; i < size && addr[i]; ++i)
                putchar(addr[i]);
        else
            for (int i = 0; i < size; ++i)
                printf("%02x ", addr[i]);
        putchar('\n');

        return 0;
    }

    int map(int argc, char *argv[])
    {
        if (argc < 2) {
            puts("Usage: map VADDR [PADDR]");
            return 1;
        }

        VAddr va = atoi(argv[1], 16);
        if (Memory::get_pte(va) != 0) {
            printf("Virtual address %08x is already mapped, are you sure? (y/N) ", va);
            char ch = getchar();
            putchar('\n');
            if (ch != 'y') return 0;
        }

        if (argc == 2) {
            PAddr pa = Memory::page_alloc(va);
            printf("Mapped virtual address %08x to physical address %08x\n", va, pa);
        } else {
            PAddr pa = atoi(argv[2], 16);
            Memory::map(va, pa);
        }

        return 0;
    }

    int unmap(int argc, char *argv[])
    {
        if (argc < 2) {
            puts("Usage: unmap VADDR");
            return 1;
        }

        VAddr va = atoi(argv[1], 16);
        if (Memory::unmap(va) != 0) {
            puts("Failed");
            return 1;
        }

        return 0;
    }

    /* Kernel shell command interpreter, copied from JOS */

    /// Support up to 16 arguments (including command name)
    const int max_argc = 16;

    /// These characters are treated as separators.
    const char whitespace[] = " \t\r\n";

    /// Parse a command line and call corresponding function.
    static int runcmd(char *cmd)
    {
        int argc = 0;
        char *argv[max_argc];

        while (true) {
            for (; *cmd && strchr(whitespace, *cmd); ++cmd)
                *cmd = '\0';
            if (*cmd == '\0')
                break;

            if (argc == max_argc - 1) {
                puts("Too many arguments");
                return 0;
            }

            argv[argc++] = cmd;

            for (; *cmd && !strchr(whitespace, *cmd); ++cmd) { }
        }

        argv[argc] = nullptr;

        if (!argc)
            return 0;

        for (int i = 0; i < commands_num; ++i)
            if (strcmp(argv[0], commands[i].name) == 0)
                return commands[i].func(argc, argv);

        fputs("Unknown command ", stdout);
        puts(argv[0]);
        return 0;
    }

    void run()
    {
        puts(CCYN "Welcome to the JOS kernel monitor!"  CEND);
        puts(CCYN "Type 'help' for a list of commands." CEND);

        while (true) {
            char *cmd = readline(CGRN "K> " CEND);
            if (cmd && runcmd(cmd) < 0)
                break;
        }
    }
}
