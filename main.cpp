#include "test.h"
#include "console.h"
#include "monitor.h"
#include "screen.h"
#include "stdio.h"

uint32_t *page_table = (uint32_t *) 0x00100000;

extern "C"
void kernel_main()
{
    page_table[0xff0] = 0x20000002;
    page_table[0xff1] = 0x20100002;
    page_table[0xff2] = 0x20200002;

    Console::init(Console::no_keyboard);

    Monitor::run();
}
