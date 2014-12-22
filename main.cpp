#include "test.h"
#include "console.h"
#include "monitor.h"

extern "C"
void kernel_main()
{
    Console::init();

    Test::lab2();

    Monitor::run();
}
