#pragma once

#include "uart.h"

/**
 * @brief A wrapper around I/O devices
 *
 * The console gets input from keyboard or UART (when keyboard not available),
 * and puts output to both UART and HDMI.
 *
 * We have a cursor system for screen output, but we will put raw characters
 * to UART and let the connected linux tools to deal with them.
 *
 * The keyboard driver is from the Cambridge tutorial rather than implemented
 * myself.
 *
 * Other modules should not do anything with I/O devices directly anymore,
 * but use this console instead. The initializer is called by kernal_main,
 * putc and getc are designed for stdio.
 */

namespace Console {
    const bool no_keyboard = false;

    /// Initialize the console (and underlying I/O devices)
    void init(bool use_keyboard = true);

    /// @brief Output a character at current position.
    ///
    /// You can use bash style escape sequence to change the color:
    /// @code
    ///     "\x1b[31mHello world!\x1b[0m" // \x1b in C equals \e in bash
    /// @endcode
    /// Or you can use the macros defined in stdio:
    /// @code
    ///     printf(CRED "Hello world!" CEND);
    /// @endcode
    ///
    /// Characters printed by putc will never be deleted.
    void putc(int ch);

    /// @brief Get a character from user and echo it back.
    ///
    /// If a USB keyboard is plugged before booting, the keyboard will be used.
    /// Otherwise we will get a character from UART. Hot-plug is not supported.
    ///
    /// Characters echoed by getc can be deleted by '\b' (or '\x7f').
    int getc();
}
