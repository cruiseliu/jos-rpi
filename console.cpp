#include "console.h"
#include "graphic.h"

namespace Console {

    // Screen size, counted by character
    static int height, width;

    // Current row and column
    static int row, col;

    void init(int h, int w, int first_row)
    {
        height = h;
        width = w;
        row = first_row;
        col = 0;

        Graphic::paint_cursor(row, col);
    }

    static inline void new_line()
    {
        // Clear the cursor
        Graphic::clear_char(row, col);

        // Scroll up the screen if full, otherwise move cursor down
        if (row == height - 1)
            Graphic::scroll_up();
        else
            ++row;

        // The new line is empty
        col = 0;
    }

    void putchar(char ch)
    {
        switch (ch) {
            case '\b': // backspace
                if (col > 0)
                    --col;
                //Graphic::clear_char(row, --col);
                break;

            case '\t': // tab
                for (int i = 0; i < 8; ++i)
                    putchar(' ');
                break;

            case '\n': // new line (enter)
                new_line();
                break;

            case '\r': // ???
                // FIXME: I don't know the exact behaviour of \r
                col = 0;
                break;

            default: // printable character
                Graphic::paint_char(row, col, ch);
                if (++col == width)
                    new_line();
        }

        Graphic::paint_cursor(row, col);
    }

    void puts(const char *str)
    {
        for (; *str; ++str)
            putchar(*str);
        putchar('\n');
    }
}
