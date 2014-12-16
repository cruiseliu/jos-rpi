#include "graphic.h"
#include "console.h"

extern "C"
void kernel_main()
{
    Graphic::init();
    Console::init(
            Graphic::height / Graphic::line_height,
            Graphic::width / Graphic::font_width,
            Graphic::logo_height / Graphic::line_height + 1
    );

    for (int i = 2; i < 8; ++i) {
        for (int j = 0; j < 16; ++j)
            Console::putchar(i * 16 + j);
        putchar('\n');
    }

    puts("\nHello, world!");
}
