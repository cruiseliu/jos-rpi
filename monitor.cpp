#include "monitor.h"
#include "stdio.h"

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

int mon_help(int argc, char *argv[])
{
    for (int i = 0; i < commands_num; ++i)
        printf("%s - %s\n", commands[i].name, commands[i].desc);
    return 0;
}

int mon_kerninfo(int argc, char *argv[])
{
    extern char _start[], __end[];

    printf("Special kernel symbols:\n");
    printf("  _start %08x\n", _start);
    printf("  __end  %08x\n", __end);
    printf("Kernel executable memory footprint: %dKB\n", (__end - _start) / 1024);
    return 0;
}

int mon_backtrace(int argc, char *argv[])
{
    register uint32_t reg_fp asm ("fp");
    uint32_t *topfp = (uint32_t *) reg_fp;
    for (int limit = 10; topfp && limit > 0; --limit) {
        uint32_t fp = topfp[-3];
        uint32_t sp = topfp[-2];
        uint32_t lr = topfp[-1];
        uint32_t pc = topfp[-0];

        printf("fp=%08x sp=%08x lr=%08x pc=%08x\n", fp, sp, lr, pc);
        topfp = (uint32_t *) fp;
    }

    return 0;
}

const int max_argc = 16;

static int strcmp(const char *p, const char *q)
{
    while (*p && *p == *q)
        ++p, ++q;
    return (int) ((unsigned char) *p - (unsigned char) *q);
}

static int runcmd(char *buf)
{
    int argc = 0;
    char *argv[max_argc];

    while (true) {
        while (*buf == ' ')
            *buf++ = '\0';
        if (*buf == '\0')
            break;

        if (argc == max_argc - 1) {
            puts("Too many arguments");
            return 0;
        }

        argv[argc++] = buf;
        while (*buf && *buf != ' ')
            ++buf;
    }

    argv[argc] = nullptr;

    if (!argc)
        return 0;

    for (int i = 0; i < commands_num; ++i)
        if (strcmp(argv[0], commands[i].name) == 0)
            return commands[i].func(argc, argv);

    puts("Unknown command");
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
