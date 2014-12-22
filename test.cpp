#include "test.h"
#include "monitor.h"
#include "stdio.h"

namespace Test {
    static void test_backtrace(int x)
    {
        printf("entering test_backtrace %d\n", x);
        if (x > 0)
            test_backtrace(x - 1);
        else
            Monitor::backtrace(0, nullptr);
        printf("leaving test_backtrace %d\n", x);
    }

    void lab1()
    {
        printf(CBLU "6828 decimal is %o octal!\n" CEND, 6828);
        test_backtrace(5);
    }

    void lab2() { }
}
