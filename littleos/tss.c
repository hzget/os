
#include "tss.h"
#include "kheap.h"

static tss_t tss;

void init_tss() {
    void *esp = kcalloc(BLOCK_SIZE);
    tss_set_kernel_stack(0x10, (uint32_t)esp + BLOCK_SIZE - 4);
}

void tss_set_kernel_stack(uint16_t segsel, uint32_t vaddr) {
    tss.esp0 = vaddr;
    tss.ss0 = segsel;
}

uint32_t tss_addr() {
    return (uint32_t)&tss;
}
