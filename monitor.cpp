#include "monitor.h"
#include "kdebug.h"
#include "stdio.h"
#include "string.h"
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
        { "help", "Display this list of commands", help },
        { "kerninfo", "Display information about the kernel", kerninfo },
        { "backtrace", "Display stack backtrace", backtrace },
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
        extern const char _start[], __start[], __end[];
        extern const char __text_start[], __rodata_start[], __data_start[], __bss_start[];
    }

    int kerninfo(int argc, char *argv[])
    {
        puts  ("Special kernel symbols:");
        printf("  _start %08x\n", _start);
        printf("  text   %08x\n", __text_start);
        printf("  rodata %08x\n", __rodata_start);
        printf("  data   %08x\n", __data_start);
        printf("  bss    %08x\n", __bss_start);
        printf("  end    %08x\n", __end);
        // It's not rounded up, this minus error should be acceptable.
        printf("Kernel executable memory footprint: %dKB\n", (__end - __start) / 1024);
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
        register uint32_t reg_fp asm ("fp");
        Addr fp {reg_fp};

        for (; fp; fp = {fp[-1]}) {
            printf("fp %08x  lr %08x\n", fp[-1], fp[0]);
            DebugInfo(fp[0] - 4).show();
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
