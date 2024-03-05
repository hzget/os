/** @file */

#ifndef PAGING_H
#define PAGING_H

#include "kheap.h"
#include <stdint.h>

// a frame is a page
#define FRAME_SIZE BLOCK_SIZE
#define PD_SIZE BLOCK_SIZE >> 2
#define PT_SIZE BLOCK_SIZE >> 2

#define PAGING_CACHE_DISABLED 16 // 0b00010000
#define PAGING_WRITE_THROUGH 8   // 0b00001000
#define PAGING_ACCESS_FROM_ALL 4 // 0b00000100
#define PAGING_IS_WRITEABLE 2    // 0b00000010
#define PAGING_IS_PRESENT 1      // 0b00000001

void init_paging();

uint32_t *create_user_pd();
void paging_switch(uint32_t *pd);
void paging_free(uint32_t *pd);
uint32_t *paging_new_directory();
void *paging_align_address(void *ptr);
uint32_t *paging_adjust_pd(uint32_t *pd);
int paging_map_to(uint32_t *pd, void *virt, void *phys, void *phys_end,
                  int flags);

#endif /* PAGING_H */
