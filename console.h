#pragma once

#include "uart.h"

namespace Console {
    // Initialize the console
    void init();

    // Output a character at current position.
    void putc(int ch);

    // Get a character from user, from UART currently
    inline int getc()
    {
        return UART::getc();
    }
}
