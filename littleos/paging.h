/** @file */

#ifndef PAGING_H
#define PAGING_H

#include "stdint.h"

void init_paging();

uint32_t *create_user_pd();
void switch_pd(uint32_t *pd);

#endif /* PAGING_H */
