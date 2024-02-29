#include "log.h"
#include "serial.h"
#include "stdio.h"
#include <stdint.h>

static void log_ui(uint32_t i) {
    /* FIXME: please make this code more beautiful */
    uint32_t n, digit;
    if (i >= 1000000000) {
        n = 1000000000;
    } else {
        n = 1;
        while (n * 10 <= i) {
            n *= 10;
        }
    }
    while (n > 0) {
        digit = i / n;
        serial_write_cell('0' + digit);
        i %= n;
        n /= 10;
    }
}

static void log_hex(char c, uint32_t i) {
    char *digits = "0123456789ABCDEF";
    if (c == 'x') {
        digits = "0123456789abcdef";
    }
    uint32_t n, digit, min_digits = 8;

    /* find the largest nibble to output */
    if (i >= 0x10000000) {
        n = 28;
    } else {
        n = 0;
        while ((((uint32_t)0x01) << (n + 4)) <= i) {
            n += 4;
        }
    }

    /* pad with zeroes */
    if (min_digits > 0) {
        min_digits -= 1;
    }
    min_digits <<= 2;
    while (min_digits > n) {
        serial_write_cell('0');
        min_digits -= 4;
    }
    /* print the number */
    while (1) {
        digit = (i >> n) & 0x0000000F;
        serial_write_cell(digits[digit]);
        if (n == 0) {
            break;
        }
        n -= 4;
    }
}

static void log_vprintf(char *fmt, va_list ap) {
    char *p;
    uint32_t uival;
    char *sval;

    for (p = fmt; *p != '\0'; ++p) {
        if (*p != '%') {
            serial_write_cell(*p);
            continue;
        }

        switch (*++p) {
        case 'c':
            uival = va_arg(ap, uint32_t);
            serial_write_cell((uint8_t)uival);
            break;
        case 'u':
            uival = va_arg(ap, uint32_t);
            log_ui(uival);
            break;
        case 'x':
        case 'X':
            uival = va_arg(ap, uint32_t);
            log_hex(*p, uival);
            break;
        case 's':
            sval = va_arg(ap, char *);
            for (; *sval; ++sval) {
                serial_write_cell(*sval);
            }
            break;
        case '%':
            serial_write_cell('%');
            break;
        }
    }
}

static void log_printf(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    log_vprintf(fmt, ap);
    va_end(ap);
}

void log_debug(char *fname, char *fmt, ...) {
    va_list ap;
    log_printf("DEBUG: %s: ", fname);
    va_start(ap, fmt);
    log_vprintf(fmt, ap);
    va_end(ap);
}

void log_info(char *fname, char *fmt, ...) {
    va_list ap;
    log_printf("INFO: %s: ", fname);
    va_start(ap, fmt);
    log_vprintf(fmt, ap);
    va_end(ap);
}

void log_error(char *fname, char *fmt, ...) {
    va_list ap;
    log_printf("ERROR: %s: ", fname);
    va_start(ap, fmt);
    log_vprintf(fmt, ap);
    va_end(ap);
}
