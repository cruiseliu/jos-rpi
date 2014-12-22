/// @file
/// @brief This file is copied from raspbootin

/* uart.cc - UART initialization & communication */
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

/* Reference material:
 * http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
 * Chapter 13: UART
 */

#include "uart.h"
#include "arm.h"
#include <cstddef>

namespace UART {
    const uint32_t gpio_base = 0x20200000;

    // Controls actuation of pull up/down to ALL GPIO pins.
    const uintptr_t gppud     = gpio_base + 0x94;
    // Controls actuation of pull up/down for specific GPIO pin.
    const uintptr_t gppudclk0 = gpio_base + 0x98;

    const uint32_t uart0_base = 0x20201000;

    const uintptr_t uart0_dr   = uart0_base + 0x00;
    const uintptr_t uart0_fr   = uart0_base + 0x18;
    const uintptr_t uart0_ibrd = uart0_base + 0x24;
    const uintptr_t uart0_fbrd = uart0_base + 0x28;
    const uintptr_t uart0_lcrh = uart0_base + 0x2c;
    const uintptr_t uart0_cr   = uart0_base + 0x30;
    const uintptr_t uart0_imsc = uart0_base + 0x38;
    const uintptr_t uart0_icr  = uart0_base + 0x44;

    /// Delay given cycles.
    // Must not be static or inline!
    void delay(int32_t cycle) {
	asm volatile("1: subs %[cycle], %[cycle], #1; bne 1b"
		     : : [cycle]"r"(cycle));
    }
    
    void init(void) {
	// Disable UART0.
	mmio_write(uart0_cr, 0x00000000);
	// Setup the GPIO pin 14 && 15.
    
	// Disable pull up/down for all GPIO pins & delay for 150 cycles.
	mmio_write(gppud, 0x00000000);
	delay(150);

	// Disable pull up/down for pin 14,15 & delay for 150 cycles.
	mmio_write(gppudclk0, (1 << 14) | (1 << 15));
	delay(150);

	// Write 0 to GPPUDCLK0 to make it take effect.
	mmio_write(gppudclk0, 0x00000000);
    
	// Clear pending interrupts.
	mmio_write(uart0_icr, 0x7FF);

	// Set integer & fractional part of baud rate.
	// Divider = UART_CLOCK/(16 * Baud)
	// Fraction part register = (Fractional part * 64) + 0.5
	// UART_CLOCK = 3000000; Baud = 115200.

	// Divider = 3000000/(16 * 115200) = 1.627 = ~1.
	// Fractional part register = (.627 * 64) + 0.5 = 40.6 = ~40.
	mmio_write(uart0_ibrd, 1);
	mmio_write(uart0_fbrd, 40);

	// Enable FIFO & 8 bit data transmissio (1 stop bit, no parity).
	mmio_write(uart0_lcrh, (1 << 4) | (1 << 5) | (1 << 6));

	// Mask all interrupts.
	mmio_write(uart0_imsc, (1 << 1) | (1 << 4) | (1 << 5) |
		    (1 << 6) | (1 << 7) | (1 << 8) |
		    (1 << 9) | (1 << 10));

	// Enable UART0, receive & transfer part of UART.
	mmio_write(uart0_cr, (1 << 0) | (1 << 8) | (1 << 9));
    }

    void putc(int byte) {
	// wait for UART to become ready to transmit
	while(true) {
	    if (!(mmio_read(uart0_fr) & (1 << 5))) {
		break;
	    }
	}
	mmio_write(uart0_dr, byte);
    }

    int getc(void) {
	// wait for UART to have recieved something
	while(true) {
	    if (!(mmio_read(uart0_fr) & (1 << 4))) {
		break;
	    }
	}
	return mmio_read(uart0_dr);
    }
}
