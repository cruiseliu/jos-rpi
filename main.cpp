#include "test.h"
#include "console.h"
#include "monitor.h"
#include "screen.h"
#include "stdio.h"

extern "C"
void kernel_main()
{
    Console::init(Console::no_keyboard);

    Monitor::run();
}
