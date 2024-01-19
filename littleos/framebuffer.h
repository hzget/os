/** @file */
/*
 * framebuffer.h provides funcs to write to framebuffer
 *
 * It take the following link as a reference:
 *
 * https://github.com/littleosbook/aenix/blob/master/src/kernel/fb.h
 *
 */

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

/** fb_write:
 *  Write a buffer of bytes to framebuffer
 *
 *  @param buf A pointer to the buffer
 *  @param len The length to write
 *  @return len The length has been written
 */
int fb_write(const char *buf, uint32_t len);

void fb_clear();
void fb_move_cursor(uint16_t row, uint16_t col);

void fb_put_b(uint8_t b);
void fb_put_s(const char *s);
#endif /* FRAMEBUFFER_H */
