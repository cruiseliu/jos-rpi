#include "monitor.h"
#include "stdio.h"
#include "string.h"
#include <cstdint>

struct Command {
    const char *name;
    const char *desc;
    int (*func)(int argc, char *argv[]);
};

const Command commands[] = {
    { "help", "Display this list of commands", mon_help },
    { "kerninfo", "Display information about the kernel", mon_kerninfo },
    { "backtrace", "mon_backtrace", mon_backtrace },
};

const int commands_num = sizeof(commands) / sizeof(Command);

/* Kernel Shell Utilities */

int mon_help(int argc, char *argv[])
{
    for (int i = 0; i < commands_num; ++i)
        printf("%s - %s\n", commands[i].name, commands[i].desc);
    return 0;
}

int mon_kerninfo(int argc, char *argv[])
{
    // _start defined in boot.S, others in kernel.ld
    extern const char _start[], __start[], __end[];
    extern const char __text_start[], __rodata_start[], __data_start[], __bss_start[];

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

int mon_backtrace(int argc, char *argv[])
{
    // A new way to get register value
    register uint32_t reg_fp asm ("fp");
    uint32_t *fp = (uint32_t *) reg_fp;

    // Documentations are not reliable. The official website[1] said stack frame
    // layout below fp should be (pc,lr,sp,fp). However I disassambled my kernel
    // and found the first instruction of each function is "push {fp, lr}".
    // The arguments are not on the stack thus invisible to backtrace.
    // [1] http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0041c/Cegbajdj.html
    for (; fp; fp = (uint32_t *) fp[-1])
        printf("fp %08x  lr %08x\n", fp[-1], fp[0]);

    // TODO: stab info

    return 0;
}

/* Kernel shell command interpreter, copied from JOS */

const int max_argc = 16;

const char whitespace[] = " \t\r\n";

static int runcmd(char *buf)
{
    int argc = 0;
    char *argv[max_argc];

    while (true) {
        for (; *buf && strchr(whitespace, *buf); ++buf)
            *buf = '\0';
        if (*buf == '\0')
            break;

        if (argc == max_argc - 1) {
            puts("Too many arguments");
            return 0;
        }

        argv[argc++] = buf;

        for (; *buf && !strchr(whitespace, *buf); ++buf) { }
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

void monitor()
{
    puts("Welcome to the JOS kernel monitor!");
    puts("Type 'help' for a list of commands.");

    while (true) {
        char *cmd = readline("K> ");
        if (cmd && runcmd(cmd) < 0)
            break;
    }
}
