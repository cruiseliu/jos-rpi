#include "console.h"

extern "C"
void kernel_main()
{
    Console::init();

    for (int i = 2; i < 8; ++i) {
        for (int j = 0; j < 16; ++j)
             putchar(i * 16 + j);
        putchar('\n');
    }

    puts("\nHello, world!");

    // debug
    while (true)
        putchar(getchar() + 1);
}
