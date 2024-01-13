#include "mem.h"

uint32_t align_up(uint32_t n, uint32_t a) {
    if (n % a == 0) {
        return n;
    }
    return n - (n % a) + a;
}

uint32_t align_down(uint32_t n, uint32_t a) { return n - (n % a); }