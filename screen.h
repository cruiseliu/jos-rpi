#pragma once

#include <cstdint>

/**
 * @brief The Raspberry Pi graphic system.
 *
 * Because we are using framebuffer (don't worry about this unless you want to touch implementation
 * details), this system is much more flexible than JOS. You can write or clear characters at any
 * position with any (24 bit) color you like. It even has the ability of displaying pictures.
 *
 * Remember to call `init` before any other functions.
 */

namespace Screen {
    /// Due to endian issue, the order is 0xAABBGGRR.
    typedef uint32_t Color;

    /// @name Predefined colors
    //@{
    const Color black   = 0xff000000;
    const Color red     = 0xff0000ff;
    const Color green   = 0xff00ff00;
    const Color yellow  = 0xff00ffff;
    const Color blue    = 0xffff0000;
    const Color magenta = 0xffff00ff;
    const Color cyan    = 0xffffff00;
    const Color gray    = 0xffb2b2b2;
    const Color white   = 0xffffffff;
    //@}

    /// Default background color
    const Color background_color = black;
    /// Default foreground (text) color
    const Color foreground_color = gray;

    /// @brief Set the screen up.
    /// This will show a logo of `logo_height` pixels high at top, you may want to avoid
    /// overlapping it.
    /// If something wrong, the function will stuck and you will see the "rainbow" screen.
    void init();

    /// Paint a character at the given *text* row and column,
    /// using given color (or foreground_color by default).
    void paint_char(int row, int column, uint8_t ch, Color color = foreground_color);

    /// Clear a character, similar to paint_char.
    void clear_char(int row, int column, Color color = background_color);

    /// Paint a cursor at given text row and column.
    static inline void paint_cursor(int row, int column)
    {
        clear_char(row, column, foreground_color);
    }

    /// Scroll the screen up by one line of text.
    void scroll_up();

    /**
     * @name Screen settings
     *
     * The width and height are hard-coded. Maybe we can detect it by some way, but
     * let's keep it simple. The color depth is 32 bits with alpha disabled in
     * config.txt. We do not use 24 bit because the GPU reads framebuffer byte by
     * byte, so we can't use a simple data type to represents a 24 bit pixel.
     */
    //@{
    const int width  = 1024;
    const int height = 768;
    // don't forget to modify Color type and Framebuffer struct if you change this
    const int depth  = 32;
    //@}

    /**
     * @name Font
     *
     * The font is named Proggy Clean, you can find it on the web.
     *
     * Each character is 7x13 pixels itself, we add one pixel after each column,
     * and three pixels before each row.
     *
     * The font contains ASCII characters '\x20' (space) to '\x7e'
     * (tiled), any other characters will be represented as a box.
     */
    //@{
    const int font_width = 8;
    const int font_height = 13;
    const int line_space = 3;
    const int line_height = font_height + line_space;

    /// The last character available in the font, assumed to be non-printable.
    const int font_last_char = 127;

    /// The bitmap font defined in font.cpp
    extern const uint16_t font[font_last_char + 1][font_height];
    //@}

    /**
     * @name Logo
     * The system will show a logo at the top-left corner.
     */
    //@{
    const int logo_width  = 88;
    const int logo_height = 88;

    /// The bitmap logo defined in logo.cpp
    extern const Color logo[logo_height][logo_width];
    //@}
}
