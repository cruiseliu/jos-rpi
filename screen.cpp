#include "screen.h"
#include "common.h"

/**
 * This file deals with the RPi graphic system.
 *
 * The VGA text-mode used by JOS is a PC specific feature, thus not available
 * for RPi or any other ARM platforms. So we have to use framebuffer even for
 * most basic text console. Fortunately I have found some good documentation
 * on the web.
 *
 * [1] quick reference: http://elinux.org/RPi_Framebuffer
 * [2] step-by-step tutorial: http://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/
 */

/**
 * Mailbox
 *
 * The CPU communicates with GPU using mailbox.
 * See [1] for details.
 */
namespace Mailbox {
    const Addr base_addr = 0x2000b880;

    // The port to receive message
    const Addr read_port   = base_addr + 0x00;
    // The port to check status
    const Addr status_port = base_addr + 0x18;
    // The port to send message
    const Addr write_port  = base_addr + 0x20;

    // This bit indicating the mailbox is empty
    const Bitset empty = 1 << 30;
    // This bit indicating the mailbox is full
    const Bitset full  = 1 << 31;

    // The last 3 bits of a "mail" is the channel
    const Bitset channel_mask = 0xf;
    // Thus only 15 channels are available
    const int max_channel = 15;

    // Send data to channel
    static inline void send(uint32_t channel, uint32_t data)
    {
        // Validate the channel
        if ((data & channel_mask) || (channel > max_channel)) return;

        // Wait until mailbox not full
        while (mmio_read(status_port) & full) { }

        // Send the mail
        mmio_write(write_port, data | channel);
    }

    // Receive data from channel, drop any messages from other channels.
    // Return -1 on fail.
    static inline uint32_t receive(uint32_t channel)
    {
        // Validate the channel
        if (channel > max_channel) return -1;

        uint32_t data;

        // Check a mail, if it's not from channel, check another one
        do {
            // Wait until mailbox not empty
            while (mmio_read(status_port) & empty) { }
            // Read a mail
            data = mmio_read(read_port);
        } while ((data & channel_mask) != channel);

        // The caller want raw mail without channel info
        return data & ~channel_mask;
    }
}

/**
 * Framebuffer related stuff.
 *
 * See [1] for details.
 */
namespace Screen {
    struct Framebuffer {
        // physical/virtual(framebuffer) width/height
        int32_t pw, ph, vw, vh;
        // number of bytes per row
        int32_t pitch;
        // bits per pixel
        int32_t depth;
        // start position
        int32_t x, y;
        // pointer to the framebuffer
        Color *pixels;
        // size of the framebuffer
        uint32_t size;
    };

    // Note the alignment.
    static Framebuffer fb __attribute__((aligned(16)));

    // The framebuffer information should be send on mailbox channel 1
    const int gpu_mailbox = 1;
    // Set the 30th bit to flush screen, said by [2]
    // TODO: What will happen if we use a high address for the kernel?
    const Bitset gpu_flush = 0x40000000;

    static inline void paint_logo();

    void init()
    {
        // Configure framebuffer
        fb.pw = fb.vw = width;
        fb.ph = fb.vh = height;
        fb.depth = depth;

        // Send it to GPU
        Mailbox::send(gpu_mailbox, (Bitset)&fb | gpu_flush);

        // [2] says GPU returns 0 on success, but [1] doesn't agree.
        // I chose to believe [2], it seems to work out fine.
        if (Mailbox::receive(gpu_mailbox) != 0)
            while (true) { } // something wrong, stuck

        paint_logo();
    }

    static inline Color get_pixel(int row, int col)
    {
        return fb.pixels[row * width + col];
    }

    // Set the pixel at (row,col) to given color.
    static inline void set_pixel(int row, int col, Color color)
    {
        fb.pixels[row * width + col] = color;
    }

    // Paint the logo.
    static inline void paint_logo()
    {
        for (int i = 0; i < logo_height; ++i)
            for (int j = 0; j < logo_width; ++j)
                set_pixel(i, j, logo[i][j]);
    }

    void paint_char(int row, int col, uint8_t ch, Color color)
    {
        // Assume font_last_char is not printable,
        // set any character with a greator ASCII code to it.
        if (ch > font_last_char) ch = font_last_char;

        // calculate the pixel position
        row = row * line_height + line_space;
        col = col * font_width;

        // paint the character
        for (int i = 0; i < font_height; ++i)
            for (int j = 0; j < font_width; ++j)
                set_pixel(row + i, col + j, (font[ch][i] >> j) & 1 ? color : background_color);
    }

    void clear_char(int row, int col, Color color)
    {
        row = row * line_height + line_space;
        col = col * font_width;
        
        for (int i = 0; i < font_height; ++i)
            for (int j = 0; j < font_width; ++j)
                set_pixel(row + i, col + j, color);
    }

    void scroll_up()
    {
        // TODO: use memmove and memset
        // move each pixel line_height up
        for (int i = 0; i < height - line_height; ++i)
            for (int j = 0; j < width; ++j)
                set_pixel(i, j, get_pixel(i + line_height, j));
        // fill the last line
        for (int i = height - line_height; i < height; ++i)
            for (int j = 0; j < width; ++j)
                set_pixel(i, j, background_color);
    }
}