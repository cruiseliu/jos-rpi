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

    void init();
    void paint_char(int row, int column, uint8_t ch, Color color = foreground_color);

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

    // The last character available in the font, it should be non-printable.
    static const int font_last_char = 127;

    // in font.cpp
    extern const uint16_t font[font_last_char + 1][font_height];

    /* Logo: The system will show a logo at the top-left corner after booting up. */
    static const int logo_width  = 88;
    static const int logo_height = 88;

    // in logo.cpp
    extern const Color logo[logo_height][logo_width];

    // The row number of first text line
    static const int first_text_row = logo_height / (font_height + line_space) + 1;
}
