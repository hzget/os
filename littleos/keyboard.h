/** @file */

#include "pic.h"

#define KEYBOARD_INTERRUPT PIC_1_START_INTERRUPT + 1

/** read_keyboard_char:
 *  Reads a character from the keyboard
 *
 *  @return The an ASCII character from the keyboard
 */
char read_keyboard_char(void);
