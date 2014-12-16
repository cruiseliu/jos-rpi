#pragma once

#include "uart.h"

namespace Console {
    // Initialize the console, the size is set to row*column characters,
    // and the current row is set to first_row.
    //void init(int row, int column, int first_row);
    void init();

    // Output a character at current position.
    void putchar(char ch);
    // Output a string at current position.
    void puts(const char *str);

    // Get a character from user, from UART currently
    inline char getchar()
    {
        return UART::getc();
    }
}

// Console is the default stdin/stdout now
using Console::putchar;
using Console::puts;
using Console::getchar;
