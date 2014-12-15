#include "graphic.h"

extern "C"
void kernel_main()
{
    Graphic::init();
    for (int i = 2; i < 8; ++i)
        for (int j = 0; j < 16; ++j)
            Graphic::paint_char(i + Graphic::first_text_row, j, i * 16 + j);
}
