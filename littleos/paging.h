/** @file */

#ifndef PAGING_H
#define PAGING_H

#include "kheap.h"
#include <stdint.h>

#define FRAME_SIZE BLOCK_SIZE
#define PD_SIZE BLOCK_SIZE >> 2
#define PT_SIZE BLOCK_SIZE >> 2

#define USER_STACK_VADDR 0xBFFFFFFB
#define USER_CODE_VADDR 0x00000000

void init_paging();

uint32_t *create_user_pd();
void switch_pd(uint32_t *pd);

#endif /* PAGING_H */
