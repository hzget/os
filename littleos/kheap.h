/** @file */

#ifndef KHEAP_H
#define KHEAP_H

#include "stddef.h"
#include "stdint.h"

typedef struct heap_table heap_table_t;
typedef struct heap heap_t;

struct heap_table {
    uint8_t *entries;
    size_t size;
};

struct heap {
    heap_table_t table;
    void *addr;
};

void *kmalloc(size_t size);
void kfree(void *ptr);
void init_kheap();
void kheap_print_table_entries(size_t n);

#endif /* KHEAP_H */
