#pragma once

namespace Console {
    // Initialize the console, the size is set to row*column characters,
    // and the current row is set to first_row.
    void init(int row, int column, int first_row);

    // Output a character at current position.
    void putchar(char ch);
    // Output a string at current position.
    void puts(const char *str);
}

// Console is the default stdout now
using Console::putchar;
using Console::puts;
