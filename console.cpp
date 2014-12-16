#include "console.h"
#include "screen.h"
#include "uart.h"

namespace Console {

    // Screen size, counted by character
    static int height, width;

    // Current row and column
    static int row, col;

    void init()
    {
        // We will output to UART and screen
        UART::init();
        Screen::init();

        height = Screen::height / Screen::line_height;
        width  = Screen::width  / Screen::font_width;

        // Set cursor below the logo
        row = Screen::logo_height / Screen::line_height + 1;
        col = 0;

        Screen::paint_cursor(row, col);
    }

    static inline void new_line()
    {
        // Clear the cursor
        Screen::clear_char(row, col);

        // Scroll up the screen if full, otherwise move cursor down
        if (row == height - 1)
            Screen::scroll_up();
        else
            ++row;

        // The new line is empty
        col = 0;
    }

    const char max_escape = 0x20;

    void putchar(char ch)
    {
        // Linux terminal can handle control charaters better than us
        UART::putc(ch);

        switch (ch) {
            case '\b': // backspace
                if (col > 0)
                    --col;
                //Screen::clear_char(row, --col);
                break;

            case '\t': // tab
                col += 8;
                if (col >= width)
                    new_line();
                break;

            case '\n': // new line (enter)
                new_line();
                break;

            case '\r': // ???
                // FIXME: I don't know the exact behaviour of \r
                col = 0;
                break;

            default: // printable character
                if (ch < max_escape) // unsupported escape code
                    return;

                Screen::paint_char(row, col, ch);
                if (++col == width)
                    new_line();
        }

        Screen::paint_cursor(row, col);
    }

    void puts(const char *str)
    {
        for (; *str; ++str)
            putchar(*str);
        putchar('\n');
    }
}
