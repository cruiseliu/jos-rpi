#include "stdio.h"
#include "console.h"
#include "monitor.h"

void test_backtrace(int x)
{
    printf("entering test_backtrace %d\n", x);
    if (x > 0)
        test_backtrace(x - 1);
    else
        mon_backtrace(0, nullptr);
    printf("leaving test_backtrace %d\n", x);
}

extern "C"
void kernel_main()
{
    Console::init();

    test_backtrace(5);

    printf("6828 decimal is %o octal!\n", 6828);

    monitor();
}
