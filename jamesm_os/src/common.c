// common.c -- Defines some global functions.
//             From JamesM's kernel development tutorials.

#include "common.h"

// Write a byte out to the specified port.
void outb(uint16_t port, uint8_t value) {
    asm volatile("outb %1, %0" : : "dN"(port), "a"(value));
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "dN"(port));
    return ret;
}

uint16_t inw(uint16_t port) {
    uint16_t ret;
    asm volatile("inw %1, %0" : "=a"(ret) : "dN"(port));
    return ret;
}

// Copy len bytes from src to dest.
void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len) {
    for (; len > 0; len--) {
        *(dest++) = *(src++);
    }
}

// Write len copies of val into dest.
void memset(uint8_t *dest, uint8_t val, uint32_t len) {
    for (; len > 0; len--) {
        *(dest++) = val;
    }
}

// Compare two strings. Should return -1 if
// str1 < str2, 0 if they are equal or 1 otherwise.
int strcmp(char *str1, char *str2) {
    while (1) {
        char c1 = *(str1++), c2 = *(str2++);
        if (c1 == c2) {
            if (c1 == '\0') {
                return 0;
            }
            continue;
        }
        return c1 < c2 ? -1 : 1;
    }
}

uint32_t strlen(const char *str) {
    int n = 0;
    while (*str != '\0') {
        n++;
    }
    return n;
}

// Copy the NULL-terminated string src into dest, and
// return dest.
char *strcpy(char *dest, const char *src) {
    char *rt = dest;
    while (*src != '\0') {
        *(dest++) = *(src++);
    }
    *dest = '\0';
    return rt;
}

// Concatenate the NULL-terminated string src onto
// the end of dest, and return dest.
char *strcat(char *dest, const char *src) {
    return strcpy(dest + strlen(dest), src);
}
