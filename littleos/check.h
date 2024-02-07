/** @file */

#ifndef CHECK_H
#define CHECK_H

#include <stdint.h>

void multiboot_check();
void kernel_check();
void page_fault_check();
void kheap_check();
void check_address_access(uint32_t *addr);
void check_disk();

#endif /* CHECK_H */
