
#ifndef STRING_H
#define STRING_H

#include "stdint.h"

void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len);
void memset(uint8_t *dest, uint8_t val, uint32_t len);
int strcmp(char *str1, char *str2);
char *strcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);
uint32_t strlen(const char *str);

#endif // STRING_H
