/** @file */

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

/** fb_write:
 *  Write a buffer of bytes to framebuffer
 *
 *  @param buf A pointer to the buffer
 *  @param len The length to write
 *  @return len The length has been written
 */
int fb_write(char *buf, unsigned int len);

#endif /* FRAMEBUFFER_H */
