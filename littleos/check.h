/** @file */

#ifndef CHECK_H
#define CHECK_H

#include "status.h"
#include <stdint.h>

#define SUCCESS(status) ((status) == STATUS_OK ? 1 : 0)
#define ERROR(value) (void *)(value)
#define ERROR_I(value) (int)(value)
#define ISERR(value) ((int)value < 0)

void multiboot_check();
void kernel_check();
void page_fault_check();
void kheap_check();
void check_address_access(uint32_t *addr);
void check_disk();
void check_streamer();
void check_pparser();
void check_fopen();

#endif /* CHECK_H */
