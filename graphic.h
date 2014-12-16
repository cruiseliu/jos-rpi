#pragma once

#include <cstdint>

// Due to endian issue, the order is 0xAABBGGRR.
typedef uint32_t Color;

namespace Graphic {
    static const Color black = 0xff000000;
    static const Color gray  = 0xffb2b2b2;
    static const Color white = 0xffffffff;

    static const Color background_color = black;
    static const Color foreground_color = gray;

    // Set the framebuffer up. The function will stuck if it fails,
    // and you will see the 4-color gradient screen.
    void init();

    // Paint a character at the given *text* row and column,
    // using given color (or foreground_color by default).
    void paint_char(int row, int column, uint8_t ch, Color color = foreground_color);

    // Clear a character, similar to paint_char.
    void clear_char(int row, int column, Color color = background_color);

    // Paint a cursor at given text row and column.
    static inline void paint_cursor(int row, int column)
    {
        clear_char(row, column, foreground_color);
    }

    // Scroll the screen up by one line of text.
    void scroll_up();

    /**
     * Screen Settings
     *
     * The width and height are hard-coded. Maybe we can detect it by some way, but
     * let's make it simple. The color depth is 32 bits with alpha disabled in
     * config.txt. We do not use 24 bit because the GPU reads framebuffer byte by
     * byte, so we can't use a simple data type to represents a 24 bit pixel.
     */
    static const int width  = 1024;
    static const int height = 768;
    // don't forget to modify Color type and Framebuffer struct if you change this
    static const int depth  = 32;

    /**
     * Font
     *
     * The font is name Proggy Clean, you can find it on the web. Each character is
     * 7x13 pixels itself. We add one pixel after each column, and three pixels
     * before each row. The font contains ASCII characters '\x20' (space) to '\x7e'
     * (tiled), any other characters are represented as a box.
     */
    static const int font_width = 8;
    static const int font_height = 13;
    static const int line_space = 3;
    static const int line_height = font_height + line_space;

    // The last character available in the font, it should be non-printable.
    static const int font_last_char = 127;

    // in font.cpp
    extern const uint16_t font[font_last_char + 1][font_height];

    /* Logo: The system will show a logo at the top-left corner after booting up. */
    static const int logo_width  = 88;
    static const int logo_height = 88;

    // in logo.cpp
    extern const Color logo[logo_height][logo_width];
}
