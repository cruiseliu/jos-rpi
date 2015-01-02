#include "screen.h"
#include "arm.h"
#include "memlayout.h"
#include "stdio.h"
#include "common.h"

typedef uint32_t Bitset;

/**
 * @brief The mechanism used to trasferring data between CPU and GPU.
 *
 * The GPU has several pre-defined mailbox channels for various purpose, including framebuffer
 * access, which uses channel 1. It seems like Raspberry Pi has more than one mailboxes, but we are
 * only concerned with the first one.
 *
 * Implementation details:
 *
 * A mailbox has up to 6 MMIO ports, 3 of them (read, write, status) are needed for our purpose.
 * Each port is shared by all channels, so we must specify the channel ID in lowest 4 bits while
 * writing a word to MMIO ports. Likewise, when we read a word it contains channel ID in the same
 * bits.
 *
 * This means there are at most 15 channels, and all "mails" are multiple of 16.
 *
 * To send a mail, query the status port until it's not full (bit 31 is 0), then write data and
 * channel to write port.
 *
 * To receive a mail, query the status port until it's not empty (bit 30 is 0), then read a word
 * from read port. If it does not contain expected channel ID, just ignore it and get another mail.
 *
 * [Official description](https://github.com/raspberrypi/firmware/wiki/Accessing-mailboxes).
 */
namespace Mailbox {
    /// The base address of mailbox MMIO
    const VAddr base_addr = peri_vaddr(0x2000b880);

    /// The port to receive message
    const VAddr read_port   = base_addr + 0x00;
    /// The port to check status
    const VAddr status_port = base_addr + 0x18;
    /// The port to send message
    const VAddr write_port  = base_addr + 0x20;

    /// This bit indicating the mailbox is empty
    const Word empty = 1 << 30;
    /// This bit indicating the mailbox is full
    const Word full  = 1 << 31;

    /// The last 4 bits of a "mail" is the channel
    const Word channel_mask = 0xf;
    /// Thus only 15 channels are available
    const int max_channel = 15;

    /// Send data to channel.
    /// The last 4 bits of data must be 0.
    static inline void send(Word channel, Word data)
    {
        // Validate the channel
        if ((data & channel_mask) || (channel > max_channel)) return;

        // Wait until mailbox not full
        while (mmio_read(status_port) & full) { }

        // Send the mail
        mmio_write(write_port, data | channel);
    }

    /// Receive data from channel.
    /// Drop any messages from other channels. Return -1 on fail.
    static inline Word receive(Word channel)
    {
        // Validate the channel
        if (channel > max_channel) return -1;

        Word data;

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
 * Implementation details:
 *
 * The VGA text-mode used by JOS is a PC specific feature, thus not available for RPi or any other
 * ARM platforms. So we have to use framebuffer even for most basic text console. Fortunately there
 * are some good documentations:
 *
 * * [1] [quick reference](http://elinux.org/RPi_Framebuffer)
 * * [2] [assembly tutorial](http://www.cl.cam.ac.uk/projects/raspberrypi/tutorials/os/)
 */
namespace Screen {
    /**
     * @brief Information of the framebuffer
     */
    struct FramebufferInfo {
        /// physical width
        int32_t pw;
        /// physical height
        int32_t ph;

        /// virtual (framebuffer) width
        int32_t vw;
        /// virtual (framebuffer) height
        int32_t vh;

        /// number of bytes per row
        int32_t pitch;

        /// bits per pixel
        int32_t depth;

        /// start X position (not used)
        int32_t x;
        /// start Y position (not used)
        int32_t y;

        /// physical address to the framebuffer
        PAddr paddr;

        /// size of the framebuffer
        uint32_t size;
    };

    /// Framebuffer information, alignment is required by mailbox.
    static FramebufferInfo fb_info __attribute__((aligned(16)));

    /// Framebuffer information should be send on mailbox channel 1.
    const int gpu_mailbox = 1;

    /// It seems like that the MMU of GPU has mapped addresses above 0x40000000 as not cached. Add
    /// this to the address of framebuffer will cause GPU to flush as soon as the framebuffer
    /// updated. But it might degrade performance. (To be confirmed)
    const VAddr gpu_flush = 0x40000000;

    /// The framebuffer.
    Color *fb;

    static inline void paint_logo();

    /// Implementation details:
    ///
    /// Fill the settings in a FramebufferInfo object and send it's physical address to GPU by
    /// mailbox. GPU will fill in its rest fields including the physical address of framebuffer,
    /// and send a mail back with content 0 if no error occurred.
    ///
    /// Once set up, we can write RGB value of each pixel to corresponding virtual address and
    /// display colors. We can also read them back.
    void init()
    {
        // Configure framebuffer
        fb_info.pw = fb_info.vw = width;
        fb_info.ph = fb_info.vh = height;
        fb_info.depth = depth;

        // Send it to GPU
        Mailbox::send(gpu_mailbox, paddr(&fb_info) + gpu_flush);

        // Did it succeed?
        Word ret = Mailbox::receive(gpu_mailbox);
        if (ret != 0) {
            //fprintf(stderr, "received %x\n", ret);
            while (true) { } // something wrong, stuck
        }

        // Map framebuffer to virtual address
        PAddr pbase = round_down(fb_info.paddr, page_size);
        mem_map(framebuffer_vbase, pbase, fb_info.paddr + fb_info.size);

        fb = (Color *) (fb_info.paddr - pbase + framebuffer_vbase);

        paint_logo();
    }

    /// Get the color of pixel at (row,col).
    static inline Color get_pixel(int row, int col)
    {
        return fb[row * width + col];
    }

    /// Set the pixel at (row,col) to given color.
    static inline void set_pixel(int row, int col, Color color)
    {
        fb[row * width + col] = color;
    }

    /// Paint the logo defined in logo.cpp
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
        // TODO: Extremely slow, how can we make it faster?

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
