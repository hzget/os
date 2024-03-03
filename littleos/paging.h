/** @file */

#ifndef PAGING_H
#define PAGING_H

#include "kheap.h"
#include <stdint.h>

#define FRAME_SIZE BLOCK_SIZE
#define PD_SIZE BLOCK_SIZE >> 2
#define PT_SIZE BLOCK_SIZE >> 2

void init_paging();

uint32_t *create_user_pd();
void paging_switch(uint32_t *pd);
void paging_free(uint32_t *pd);

#endif /* PAGING_H */
