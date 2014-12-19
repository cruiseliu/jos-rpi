#include "console.h"
#include "screen.h"
#include "uart.h"

extern "C" {
    /// Initialize the USB driver, this will take several seconds.
    void UsbInitialise();
    /// Count keyboards on USB port.
    int KeyboardCount();
    /// Refresh keyboard status, including keystroke status.
    void KeyboardUpdate();
    /// Get the keystoke at the moment of last KeyboardUpdate call.
    int KeyboardGetChar();
}

namespace Console {

    /// @name Screen size, counted by characters
    //@{
    static int height, width;
    //@}

    /// @name Current cursor position
    //@{
    static int row, col;
    //@}

    /// Whether or not a keyboard is plugged before booting
    static bool keyboard_avail;

    void init()
    {
        UART::init();
        Screen::init();

        UsbInitialise();
        keyboard_avail = KeyboardCount();

        height = Screen::height / Screen::line_height;
        width  = Screen::width  / Screen::font_width;

        // Put cursor below the logo
        row = Screen::logo_height / Screen::line_height + 1;
        col = 0;

        Screen::paint_cursor(row, col);
    }

    /// Create a new line on the screen.
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

    /// Character next to last control character (' ', whitespace)
    const char max_control = 0x20;

    void putc(int ch)
    {
        // Linux terminals can handle control charaters better than us
        UART::putc(ch);

        switch (ch) {
            case '\b': // backspace
                // Clear the cursor
                Screen::clear_char(row, col);
                if (col > 0)
                    --col;
                // Deleted character is covered by cursor
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

    int getc()
    {
        // Keyboard not plugged, use UART
        if (!keyboard_avail)
            return UART::getc();

        // Wait for a keystroke
        int ret;
        do { 
            KeyboardUpdate();
            ret = KeyboardGetChar();
        } while (ret == 0);
        return ret;
    }
}
