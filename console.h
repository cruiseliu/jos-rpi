#pragma once

#include "uart.h"

/**
 * @brief A wrapper around I/O devices
 *
 * The console gets input from UART, and puts output to both UART and HDMI.
 * Ideally it should also read from keyboard, but the driver has not been
 * implemented yet.
 *
 * We have a cursor system for screen output, but we will put raw characters
 * to UART and let the connected linux tools to deal with them.
 *
 * Other modules should not do anything with I/O devices directly anymore,
 * but use this console instead. The initializer is called by kernal_main,
 * putc and getc are designed for stdio.
 */

namespace Console {
    /// Initialize the console (and underlying I/O devices)
    void init();

    /// Output a character at current position.
    void putc(int ch);

    /// Get a character from user, from UART currently.
    inline int getc()
    {
        return UART::getc();
    }
}
