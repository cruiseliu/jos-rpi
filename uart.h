/// @file
/// @brief This file is copied from raspbootin

/* uart.h - UART initialization & communication */
/* Copyright (C) 2013 Goswin von Brederlow <goswin-v-b@web.de>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#pragma once

#include <cstdint>

namespace UART {
    /// @brief Initialize UART0.
    void init();

    /// @brief Transmit a byte via UART0.
    void putc(int byte);

    /// @brief Receive a byte via UART0.
    int getc();

    /// @brief Transmit a string.
    void puts(const char *str);

    /// @brief Transmit a hex number in string form.
    void putx(uint32_t num);
}
