#include "console.h"
#include "screen.h"
#include "uart.h"

/**
 * Console: a wrapper arount I/O devices
 *
 * The console gets input from UART, and puts output to both UART and HDMI.
 * Ideally it should also read from keyboard, but the driver has not been
 * implemented yet.
 * We have a cursor system for screen output, but we will put raw characters
 * to UART and let the connected linux tools to deal with them.
 * Other modules should not do anything with I/O devices directly anymore,
 * but use this console instead. The initializer is called by kernal_main,
 * putc and getc are designed for stdio.
 */

namespace Console {

    // Screen size, counted by character
    static int height, width;

    // Current row and column
    static int row, col;

    // Initialize the console and underlying I/O devices.
    void init()
    {
        UART::init();
        Screen::init();

        height = Screen::height / Screen::line_height;
        width  = Screen::width  / Screen::font_width;

        // Put cursor below the logo
        row = Screen::logo_height / Screen::line_height + 1;
        col = 0;

        Screen::paint_cursor(row, col);
    }

    // Create a new line on the screen.
    static inline void new_line()
    {
        // Clear the cursor
        Screen::clear_char(row, col);

        // Scroll up the screen if it's full, otherwise move cursor down
        if (row == height - 1)
            Screen::scroll_up();
        else
            ++row;

        // The new line is empty
        col = 0;
    }

    const char max_control = 0x20; // ' '

    void putc(int ch)
    {
        // Linux terminals can handle control charaters better than us
        UART::putc(ch);

        switch (ch) {
            case '\b': // backspace
                if (col > 0)
                    --col;
                // The character is covered by cursor
                //Screen::clear_char(row, --col);
                break;

            case '\t': // tab
                // I don't like tab, just print up to 8 spaces
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

            default:
                // unsupported control code
                if (ch < max_control)
                    return;

                // printable character
                Screen::paint_char(row, col, ch);
                if (++col == width)
                    new_line();
        }

        Screen::paint_cursor(row, col);
    }
}
