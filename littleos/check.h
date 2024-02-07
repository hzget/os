/** @file */

#ifndef CHECK_H
#define CHECK_H

#include "status.h"
#include <stdint.h>

#define SUCCESS(status) ((status) == STATUS_OK ? 1 : 0)

void multiboot_check();
void kernel_check();
void page_fault_check();
void kheap_check();
void check_address_access(uint32_t *addr);
void check_disk();
void check_streamer();
void check_pparser();

#endif /* CHECK_H */
