#include "paging.h"
#include "common.h"
#include "constants.h"
#include "kheap.h"
#include "stdio.h"

// A bitset of frames - used or free.
static uint32_t *frames;
static uint32_t nframes;

static page_directory_t *kernel_directory;

// Defined in kheap.c
extern uint32_t g_placement_address;

#define INDEX_FROM_BIT(a) ((a) / (8 * 4))
#define OFFSET_FROM_BIT(a) ((a) % (8 * 4))

static void set_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr / 0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    frames[idx] |= (0x1 << off);
}

static void clear_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr / 0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    frames[idx] &= ~(0x1 << off);
}

static uint32_t first_free_frame() {
    uint32_t i, j;
    for (i = 0; i < INDEX_FROM_BIT(nframes); i++) {
        if (frames[i] != 0xFFFFFFFF) // nothing free, exit early.
        {
            // at least one bit is free here.
            for (j = 0; j < 32; j++) {
                uint32_t toTest = 0x1 << j;
                if (!(frames[i] & toTest)) {
                    return i * 4 * 8 + j;
                }
            }
        }
    }
    return (uint32_t)-1;
}

void alloc_frame(page_t *page, int is_kernel, int is_writeable) {
    if (!page) {
        return;
    }
    if (page->frame != 0) {
        return;
    } else {
        uint32_t idx = first_free_frame();
        if (idx == (uint32_t)-1) {
            PANIC("No free frames!");
        }
        set_frame(idx * 0x1000);
        page->present = 1;
        page->rw = (is_writeable) ? 1 : 0;
        page->user = (is_kernel) ? 0 : 1;
        page->frame = idx;
    }
}

void free_frame(page_t *page) {
    uint32_t frame;
    if (!(frame = page->frame)) {
        return;
    } else {
        clear_frame(frame);
        page->frame = 0x0;
    }
}

static void create_bitmap() {
    // The size of physical memory. For the moment we
    // we get it from multiboot sturcutre
    uint32_t mem_end_page = 0x002E0000;

    nframes = mem_end_page / 0x1000;
    frames = (uint32_t *)kmalloc(INDEX_FROM_BIT(nframes));
    memset((uint8_t *)frames, 0, INDEX_FROM_BIT(nframes) * 4);
    printf("%s: nframes=%x, addr=%x\n", __func__, nframes, frames);
}

static void print_bitmap() {
    uint32_t index = INDEX_FROM_BIT(nframes);
    uint32_t i;
    printf("%s allocated:\n", __func__);
    for (i = 0; i < index * 4; i += 4) {
        if (!frames[i]) {
            return;
        }
        printf("%x %x %x %x\n", frames[i], frames[i + 1], frames[i + 2],
               frames[i + 3]);
    }
}

static void create_page_directory() {
    kernel_directory = (page_directory_t *)kmalloc_a(sizeof(page_directory_t));
    memset((uint8_t *)kernel_directory, 0, sizeof(page_directory_t));
}

/** switch_page_directory
 * Causes the specified page directory to be loaded into the
 * CR3 register.
 */
void switch_page_directory(uint32_t addr) {
    uint32_t cr0, cr3;
    __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
    __asm__ volatile("mov %0, %%cr3" ::"r"(addr));
    printf("%s: cr3 %x ---> %x\n", __func__, cr3, addr);

    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; // Enable paging!
    __asm__ volatile("mov %0, %%cr0" ::"r"(cr0));
}

void initialise_paging() {
    create_bitmap();
    create_page_directory();

    // NOTE that we use a while loop here deliberately.
    // inside the loop body we actually change g_placement_address
    // by calling kmalloc(). A while loop causes this to be
    // computed on-the-fly rather than once at the start.
    uint32_t i = 0 + KERNEL_START_VADDR;
    while (i < g_placement_address) {
        alloc_frame(get_page(i, 1, kernel_directory), 1, 0);
        i += 0x1000;
    }

    print_bitmap();

    uint32_t addr =
        (uint32_t)&kernel_directory->tablesPhysical - KERNEL_START_VADDR;
    switch_page_directory(addr);
}

page_t *get_page(uint32_t address, int make, page_directory_t *dir) {
    address /= 0x1000;
    uint32_t table_idx = address / 1024;
    if (dir->tables[table_idx]) {
        // If this table is already assigned
        return &dir->tables[table_idx]->pages[address % 1024];
    } else if (make) {
        uint32_t tmp;
        dir->tables[table_idx] =
            (page_table_t *)kmalloc_ap(sizeof(page_table_t), &tmp);
        memset((uint8_t *)(dir->tables[table_idx]), 0, 0x1000);
        dir->tablesPhysical[table_idx] = tmp | 0x7; // PRESENT, RW, US.
        return &dir->tables[table_idx]->pages[address % 1024];
    } else {
        return 0;
    }
}
