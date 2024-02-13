
#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdint.h>

int memcmp(const uint8_t *s1, const uint8_t *s2, size_t n);
void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len);
void memset(uint8_t *dest, uint8_t val, uint32_t len);
int strcmp(char *str1, char *str2);
int strncmp(const char *s1, const char *s2, size_t n);
int strncasecmp(const char *s1, const char *s2, size_t n);
char *strcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);
size_t strlen(const char *s);
size_t strnlen(const char *s, size_t maxlen);

int isdigit(char c);
int tonumericdigit(char c);
char tolower(const char c);
#endif // STRING_H
