
#include "string.h"

// Copy len bytes from src to dest.
void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len) {
    for (; len > 0; len--) {
        *(dest++) = *(src++);
    }
}

/* memcmp()
 * compares the first n bytes of the memory areas s1 and s2.
 *
 * @return integer <, ==, or > zero if the  first n bytes
 * of s1 is found, respectively, to be less than, to match, or be
 * greater than the first n bytes of s2.
 */
int memcmp(const uint8_t *s1, const uint8_t *s2, size_t n) {
    for (size_t i = 0; i < n; i++) {
        if (*s1 == '\0' || *s2 == '\0') {
            return *s1 - *s2;
        }
        if (*s1 == *s2) {
            s1++;
            s2++;
            continue;
        }
        return *s1 - *s2;
    }
    return 0;
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

size_t strlen(const char *s) {
    int n = 0;
    while (*s != '\0') {
        s++;
        n++;
    }
    return n;
}

/* strnlen()
 * get the number of bytes in the string pointed to by s,
 * excluding the terminating null byte  ('\0'),  but  at  most  maxlen.
 *
 * In  doing  this, strnlen() looks only at the first maxlen characters
 * in the string pointed to by s and never beyond s[maxlen-1].
 */
size_t strnlen(const char *s, size_t maxlen) {
    size_t n = 0;
    while (*s != '\0') {
        s++;
        n++;
        if (n == maxlen - 1) {
            return n;
        }
    }
    return n;
}

int isdigit(char c) {
    return c >= 48 && c <= 57;
}
int tonumericdigit(char c) {
    return c - 48;
}
