
#include "common.h"

// Write len copies of val into dest.
void memset(uint8_t *dest, uint8_t val, uint32_t len) {
    uint8_t *temp = (uint8_t *)dest;
    for (; len != 0; len--)
        *temp++ = val;
}
