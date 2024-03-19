
#ifndef STDIO_H
#define STDIO_H

/* Format a string and print it on the screen, just like the libc
   function printf. */
void printf(const char *format, ...);

#define PANIC(msg)                                                             \
    do {                                                                       \
        printf("PANIC at %s()!! ", __func__);                                  \
        printf(msg);                                                           \
        for (;;) {                                                             \
        }                                                                      \
    } while (1)

#define TMP_LOG() printf("at %s() line %d\n", __func__, __LINE__)

#endif // STDIO_H
