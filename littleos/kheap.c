
#include "kheap.h"
#include "mem.h"
#include "stdio.h"
#include "string.h"

/* heap table entry:
 *  ----------------------------
 *  | has_n |  -  |  -  | used |
 *  ----------------------------
 *  |   0   | 000 | 000 |  0   |  -- 0x00 free block
 *  |   0   | 000 | 000 |  1   |  -- 0x01 used, no more block
 *  |   1   | 000 | 000 |  1   |  -- 0x81 used, more on the next entry
 */

#define HEAP_BLOCK_USED 1 << 0
#define HEAP_BLOCK_MORE 1 << 7

extern uint32_t kernel_heap_virtual_address;

static heap_t kheap;

static void clear_block_entry(size_t index);
static void clear_block_memory(size_t index);
static size_t get_num_of_blocks(size_t size);
static int get_first_block_entry(size_t n);
static void set_block_entry(int index, int flag);
static void *get_block_addr(int index);
static size_t get_block_index(void *addr);

void *kmalloc(size_t size) {
    size_t n = get_num_of_blocks(size);
    int index = get_first_block_entry(n);
    if (index < 0) {
        PANIC("not enough memory space");
    }

    int i = 0;
    while (n > 0) {
        set_block_entry(index + i, HEAP_BLOCK_USED);
        if (n > 1) {
            set_block_entry(index + i, HEAP_BLOCK_MORE);
        }
        i++;
        n--;
    }

    return get_block_addr(index);
}

void *kcalloc(size_t size) {
    void *ptr = kmalloc(size);
    memset(ptr, 0, size);
    return ptr;
}

void kfree(void *ptr) {

    if (ptr < kheap.addr) {
        PANIC("ptr is illegal");
    }

    size_t i = get_block_index(ptr);
    for (; i < kheap.table.size; i++) {
        uint8_t entry = kheap.table.entries[i];
        clear_block_entry(i);
        clear_block_memory(i);
        if (!(entry & HEAP_BLOCK_MORE)) {
            break;
        }
    }
}

static size_t get_num_of_blocks(size_t size) {
    if (size == 0) {
        return 0;
    }

    int m = size % BLOCK_SIZE;
    if (m == 0) {
        return size / BLOCK_SIZE;
    }

    return size / BLOCK_SIZE + 1;
}

static int get_first_block_entry(size_t n) {
    // printf("%s: need %u blocks\n", __func__, n);
    int start = -1, i = 0;
    size_t count = 0;
    if (n < 1) {
        goto end;
    }
    for (i = 0; (size_t)i < kheap.table.size; i++) {
        if ((kheap.table.entries[i] & HEAP_BLOCK_USED)) {
            start = -1;
            count = 0;
            continue;
        }

        if (start == -1) {
            start = i;
        }

        count++;
        if (count == n) {
            break;
        }
    }

end:
    return start;
}

static void set_block_entry(int index, int flag) {
    kheap.table.entries[index] |= flag;
}

static void *get_block_addr(int index) {
    return (uint8_t *)kheap.addr + (BLOCK_SIZE * index);
}

static size_t get_block_index(void *addr) {
    size_t offset = (uint8_t *)addr - (uint8_t *)kheap.addr;
    if (offset % BLOCK_SIZE == 0) {
        return offset / BLOCK_SIZE;
    }
    return offset / BLOCK_SIZE + 1;
}

static void clear_block_entry(size_t index) {
    kheap.table.entries[index] = 0;
}

static void clear_block_memory(size_t index) {
    memset((uint8_t *)get_block_addr(index), 0, BLOCK_SIZE);
}

void init_kheap() {
    kheap.table.entries = (uint8_t *)&kernel_heap_virtual_address;
    kheap.table.size = HEAP_SIZE / BLOCK_SIZE;
    memset(kheap.table.entries, 0x0, kheap.table.size);

    uint32_t addr = (uint32_t)(kheap.table.entries + kheap.table.size);
    kheap.addr = (void *)align_up(addr, BLOCK_SIZE);
    memset(kheap.addr, 0x0, HEAP_SIZE);

    printf("heap addr:0x%08x ~ 0x%08x\n\t table.entries 0x%08x, table.size "
           "0x%04x\n",
           kheap.addr, (uint8_t *)kheap.addr + kheap.table.size * BLOCK_SIZE,
           kheap.table.entries, kheap.table.size);
}

void kheap_print_table_entries(size_t n) {
    size_t i = 0;
    printf("kheap table entries:\n");
    for (; i < kheap.table.size - 7; i += 8) {
        uint8_t *entries = kheap.table.entries;
        printf("0x %02x %02x %02x %02x ", entries[i], entries[i + 1],
               entries[i + 2], entries[i + 3]);
        printf("%02x %02x %02x %02x\n", entries[i + 4], entries[i + 5],
               entries[i + 6], entries[i + 7]);
        if (i + 8 >= n) {
            break;
        }
    }
}
