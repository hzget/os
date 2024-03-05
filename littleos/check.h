/** @file */

#ifndef CHECK_H
#define CHECK_H

#include "status.h"
#include <stdint.h>

#define SUCCESS(status) ((status) == STATUS_OK ? 1 : 0)
#define ERROR(value) (void *)(value)
#define ERROR_I(value) (int)(value)
#define ISERR(value) ((int)value < 0)

void check_multiboot();
void check_kernel();
void check_page_fault();
void check_kheap();
void check_address_access(uint32_t *addr);
void check_disk();
void check_streamer();
void check_pparser();
void check_fopen();
void check_task();
void check_process();

#endif /* CHECK_H */
