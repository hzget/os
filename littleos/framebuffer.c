/** @file */
#include "io.h"

/* The I/O ports */
#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15

#define FB_GREEN 2
#define FB_DARK_GREY 8

#define FRAMEBUFFER_ADDRESS 0x000B8000

static unsigned short __fb_present_pos = 0x0;

/** fb_write_cell:
 *  Writes a character with the given foreground and background to position i
 *  in the framebuffer.
 *
 *  @param i  The location in the framebuffer
 *  @param c  The character
 *  @param fg The foreground color
 *  @param bg The background color
 */
static void fb_write_cell(unsigned int i, char c, unsigned char fg,
                          unsigned char bg) {
    char *fb = (char *)FRAMEBUFFER_ADDRESS;
    fb[i] = c;
    fb[i + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
}

/** fb_move_cursor:
 *  Moves the cursor of the framebuffer to the given position
 *
 *  @param pos The new position of the cursor
 */
static void fb_move_cursor(unsigned short pos) {
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT, ((pos >> 8) & 0x00FF));
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT, pos & 0x00FF);
    __fb_present_pos = pos;
}

/** fb_write:
 *  Write a buffer of bytes to framebuffer
 *
 *  @param buf A pointer to the buffer
 *  @param len The length to write
 *  @return len The length has been written
 */
int fb_write(char *buf, unsigned int len) {
    unsigned int i;
    for (i = 0; i < len; i++) {
        /* Write character at position i */
        fb_write_cell(2 * __fb_present_pos, buf[i], FB_GREEN, FB_DARK_GREY);
        fb_move_cursor(__fb_present_pos + 1);
    }
    return len;
}
