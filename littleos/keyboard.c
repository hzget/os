/** @file */

#include "interrupts.h"
#include "io.h"
#include "stdio.h"

#define KBD_DATA_PORT 0x60

/** read_scan_code:
 *  Reads a scan code from the keyboard
 *
 *  @return The scan code (NOT an ASCII character!)
 */
static unsigned char read_scan_code(void) {
    return inb(KBD_DATA_PORT);
}

static char kbd_US[128] = {
    0,   27,   '1', '2',  '3',  '4',  '5',  '6', '7',
    '8', '9',  '0', '-',  '=',  '\b', '\t', /* <-- Tab */
    'q', 'w',  'e', 'r',  't',  'y',  'u',  'i', 'o',
    'p', '[',  ']', '\n', 0, /* <-- control key */
    'a', 's',  'd', 'f',  'g',  'h',  'j',  'k', 'l',
    ';', '\'', '`', 0,    '\\', 'z',  'x',  'c', 'v',
    'b', 'n',  'm', ',',  '.',  '/',  0,    '*', 0, /* Alt */
    ' ',                                            /* Space bar */
    0,                                              /* Caps lock */
    0,                                              /* 59 - F1 key ... > */
    0,   0,    0,   0,    0,    0,    0,    0,   0, /* < ... F10 */
    0,                                              /* 69 - Num lock*/
    0,                                              /* Scroll Lock */
    0,                                              /* Home key */
    0,                                              /* Up Arrow */
    0,                                              /* Page Up */
    '-', 0,                                         /* Left Arrow */
    0,   0,                                         /* Right Arrow */
    '+', 0,                                         /* 79 - End key*/
    0,                                              /* Down Arrow */
    0,                                              /* Page Down */
    0,                                              /* Insert Key */
    0,                                              /* Delete Key */
    0,   0,    0,   0,                              /* F11 Key */
    0,                                              /* F12 Key */
    0, /* All other keys are undefined */
};

/** read_keyboard_char:
 *  Reads a character from the keyboard
 *
 *  @return The an ASCII character from the keyboard
 */
static char read_keyboard_char(void) {
    unsigned char code = read_scan_code();
    if (code & 0x80) {
        // ignore key release event
        return 0;
    }
    return kbd_US[code];
}

static void *kb_handler(struct interrupt_frame *frame) {
    (void)frame;

    char key = read_keyboard_char();
    if (key == 0x0) {
        // ignore non-printable char
        return 0;
    }
    char buffer[2];
    buffer[0] = key;
    buffer[1] = '\0';
    printf(buffer);
    return 0;
}

void init_kb() {
    register_interrupt_handler(IRQ1, kb_handler);
}
