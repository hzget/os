/** @file */
#include "framebuffer.h"
#include "constants.h"
#include "io.h"

/* The I/O ports */
#define FB_CURSOR_COMMAND_PORT 0x3D4
#define FB_CURSOR_DATA_PORT 0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15

#define FRAMEBUFFER_ADDRESS (0x000B8000 + KERNEL_START_VADDR)

/* Some screen stuff. */
#define FB_NUM_COLS 80
#define FB_NUM_ROWS 25

#define BLACK_ON_WHITE 0x0F

#define TO_ADDRESS(row, col) (fb + 2 * (row * FB_NUM_COLS + col))

#define FB_BACKSPACE_ASCII 8

static uint8_t *fb = (uint8_t *)FRAMEBUFFER_ADDRESS;
static uint16_t cursor_pos = 0x0;

static uint8_t read_cell(uint32_t row, uint32_t col) {
    uint8_t *cell = TO_ADDRESS(row, col);
    return *cell;
}

static void write_cell(uint8_t *cell, uint8_t b) {
    cell[0] = b;
    cell[1] = BLACK_ON_WHITE;
}

static void write_at(uint8_t b, uint32_t row, uint32_t col) {
    uint8_t *cell = TO_ADDRESS(row, col);
    write_cell(cell, b);
}

static void set_cursor(uint16_t loc) {
    outb(FB_CURSOR_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_CURSOR_DATA_PORT, loc >> 8);
    outb(FB_CURSOR_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_CURSOR_DATA_PORT, loc);
}

static void move_cursor_forward(void) {
    cursor_pos++;
    set_cursor(cursor_pos);
}

static void move_cursor_back(void) {
    if (cursor_pos != 0) {
        cursor_pos--;
        set_cursor(cursor_pos);
    }
}

static void move_cursor_down() {
    cursor_pos += FB_NUM_COLS;
    set_cursor(cursor_pos);
}

static void move_cursor_start() {
    cursor_pos -= cursor_pos % FB_NUM_COLS;
    set_cursor(cursor_pos);
}

static void scroll() {
    uint32_t r, c;
    for (r = 1; r < FB_NUM_ROWS; ++r) {
        for (c = 0; c < FB_NUM_COLS; ++c) {
            write_at(read_cell(r, c), r - 1, c);
        }
    }

    for (c = 0; c < FB_NUM_COLS; ++c) {
        write_at(' ', FB_NUM_ROWS - 1, c);
    }
}

void fb_put_b(uint8_t b) {
    if (b != '\n' && b != '\t' && b != FB_BACKSPACE_ASCII) {
        uint8_t *cell = fb + 2 * cursor_pos;
        write_cell(cell, b);
    }

    if (b == '\n') {
        move_cursor_down();
        move_cursor_start();
    } else if (b == FB_BACKSPACE_ASCII) {
        move_cursor_back();
        uint8_t *cell = fb + 2 * cursor_pos;
        write_cell(cell, ' ');
    } else if (b == '\t') {
        int i;
        for (i = 0; i < 4; ++i) {
            fb_put_b(' ');
        }
    } else {
        move_cursor_forward();
    }

    if (cursor_pos >= FB_NUM_COLS * FB_NUM_ROWS) {
        scroll();
        fb_move_cursor(24, 0);
    }
}

void fb_put_s(const char *s) {
    while (*s != '\0') {
        fb_put_b(*s++);
    }
}

void fb_clear() {
    uint8_t i, j;
    for (i = 0; i < FB_NUM_ROWS; ++i) {
        for (j = 0; j < FB_NUM_COLS; ++j) {
            write_at(' ', i, j);
        }
    }
    fb_move_cursor(0, 0);
}

/** fb_move_cursor:
 *  Moves the cursor of the framebuffer to the given position
 *
 *  @param row row number of new position
 *  @param col column number of new position
 */
void fb_move_cursor(uint16_t row, uint16_t col) {
    uint16_t loc = row * FB_NUM_COLS + col;
    cursor_pos = loc;
    set_cursor(loc);
}

/** fb_write:
 *  Write a buffer of bytes to framebuffer
 *
 *  @param buf A pointer to the buffer
 *  @param len The length to write
 *  @return len The length has been written
 */
int fb_write(const char *buf, uint32_t len) {
    (void)len;
    fb_put_s(buf);
    return len;
}
