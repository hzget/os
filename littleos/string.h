
#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdint.h>

int memcmp(const void *s1, const void *s2, size_t n);
void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len);
void *memset(void *s, uint8_t c, size_t n);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
int strncasecmp(const char *s1, const char *s2, size_t n);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
char *strcat(char *dest, const char *src);
size_t strlen(const char *s);
size_t strnlen(const char *s, size_t maxlen);

int isdigit(char c);
int tonumericdigit(char c);
char tolower(const char c);
#endif // STRING_H
