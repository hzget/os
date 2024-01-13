
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

#endif // STDIO_H
