/** @file */

/** serial_write:
 *
 *  Write a buffer of bytes to serial com1
 *
 *  @param buf pointer to a buffer
 *  @param len length of the buffer
 *  @return len length that has been written
 */
int serial_write(char *buf, unsigned int len);
