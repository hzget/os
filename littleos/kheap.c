#include "kheap.h"
#include "constants.h"

// Defined in link.ld
extern uint32_t kernel_virtual_end;

uint32_t g_placement_address = (uint32_t)&kernel_virtual_end;

uint32_t kmalloc(uint32_t sz) {
    uint32_t tmp = g_placement_address;
    g_placement_address += sz;
    return tmp;
}

uint32_t kmalloc_a(uint32_t sz) {
    if (g_placement_address & 0xFFFFF000) {
        g_placement_address &= 0xFFFFF000;
        g_placement_address += 0x1000;
    }
    uint32_t tmp = g_placement_address;
    g_placement_address += sz;
    return tmp;
}

uint32_t kmalloc_ap(uint32_t sz, uint32_t *phys) {
    if (g_placement_address & 0xFFFFF000) {
        g_placement_address &= 0xFFFFF000;
        g_placement_address += 0x1000;
    }
    if (phys) {
        *phys = g_placement_address - KERNEL_START_VADDR;
    }
    uint32_t tmp = g_placement_address;
    g_placement_address += sz;
    return tmp;
}
