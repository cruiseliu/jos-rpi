#include "test.h"
#include "console.h"
#include "monitor.h"
#include "screen.h"
#include "stdio.h"
#include "memory.h"

extern "C"
void kernel_main()
{
    Memory::init();

    Console::init(Console::no_keyboard);

    Monitor::run();
}
