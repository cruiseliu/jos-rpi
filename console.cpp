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

    /// '\b' can only delete characters after barrier.
    static int barrier;

    /// Whether or not a keyboard is plugged before booting
    static bool keyboard_avail;

    void init(bool use_keyboard)
    {
        UART::init();
        Screen::init();

        if (use_keyboard) {
            UsbInitialise();
            keyboard_avail = KeyboardCount();
        }

        height = Screen::height / Screen::line_height;
        width  = Screen::width  / Screen::font_width;

        // Put cursor below the logo
        row = Screen::logo_height / Screen::line_height + 1;
        col = 0;
        barrier = 0;

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

    /// Current foreground color
    Screen::Color color = Screen::foreground_color;

    /// Character next to last control character (' ', whitespace)
    const char max_control = 0x20;

    /// Output a character, without escaping handler
    static void putc_no_escape(int ch)
    {
        switch (ch) {
            case '\t': // tab
                // I don't like tab, just print up to 8 spaces
                col += 8;
                if (col >= width)
                    new_line();
                break;

            case '\n': // new line (enter)
                new_line();
                break;

            default:
                // unsupported control code
                if (ch < max_control)
                    return;

                // printable character
                Screen::paint_char(row, col, ch, color);
                if (++col == width)
                    new_line();
        }

        Screen::paint_cursor(row, col);
    }

    /// Escape sequence status
    enum EscapeMode { Normal, Entering, Escaped };
    /// Current escape mode
    static EscapeMode esc_mode = Normal;

    /// Max escape sequence size
    const int max_esc_size = 8;

    /// Current escape sequence size
    static int esc_size = 0;
    /// The sequence of escape codes
    static int esc_seq[max_esc_size];

    /// Map bash style color code to pixel color format
    const Screen::Color color_list[8] = {
        Screen::black,
        Screen::red,
        Screen::green,
        Screen::yellow,
        Screen::blue,
        Screen::magenta,
        Screen::cyan,
        Screen::gray
    };

    /// Print a character without setting barrier.
    static void putc_deletable(int ch)
    {
        if (ch == '\b') { // backspace or delete
            if (col > barrier) {
                // Sometimes UART only move the cursor without wiping character for backspace, so
                // we move the cursor, print a space to cover the character, and move cursor back.
                UART::putc('\b');
                UART::putc(' ');
                UART::putc('\b');

                // Clear the cursor and print it at left, covering the deleted character.
                Screen::clear_char(row, col);
                Screen::paint_cursor(row, --col);
            }
            return;
        }

        // Linux terminals can handle escape and control charaters better than us
        UART::putc(ch);

        if (ch == '\x1b') {
            esc_mode = Entering;
            // drop unfinished escape sequence
            esc_size = 0;

        } else if (esc_mode == Entering) { // '\x1b' but no '['
            if (ch == '[')
                esc_mode = Escaped;

            else {
                // Invalid escape sequence, ignore \x1b
                esc_mode = Normal;
                putc_no_escape(ch);
            }

        } else if (esc_mode == Escaped) {
            if ('0' <= ch && ch <= '9') {
                if (esc_size == 0)
                    esc_seq[esc_size++] = ch - '0';
                else {
                    esc_seq[esc_size - 1] *= 10;
                    esc_seq[esc_size - 1] += ch - '0';
                }

            } else if (ch == ';') {
                if (esc_size < max_esc_size)
                    esc_size++;
                // else drop last number
                esc_seq[esc_size] = 0;

            } else if (ch == 'm') { // color
                for (int i = 0; i < esc_size; ++i) {
                    if (esc_seq[i] == 0)
                        color = Screen::foreground_color;
                    else if (30 <= esc_seq[i] && esc_seq[i] <= 37)
                        color = color_list[esc_seq[i] - 30];
                    // other values (including background) not supported
                }
                esc_mode = Normal;

            } else // unsupported escape sequence, do nothing
                esc_mode = Normal;

        } else
            putc_no_escape(ch);
    }

    void putc(int ch)
    {
        putc_deletable(ch);
        barrier = col;
    }

    int getc()
    {
        int ret;
        if (!keyboard_avail) {
            // Keyboard not plugged, use UART
            ret = UART::getc();
        } else {
            // Wait for a keystroke
            do { 
                KeyboardUpdate();
                ret = KeyboardGetChar();
            } while (ret == 0);
        }
        putc_deletable(ret); // echo
        return ret;
    }
}
