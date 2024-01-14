/** @file */

#include "paging.h"
#include "constants.h"
#include "interrupts.h"
#include "stdio.h"

extern uint32_t kernel_pd;
extern void set_pd(uint32_t pdt_paddr);
extern void invalidate_page_table_entry(uint32_t *vaddr);

static void pd_mmap(uint32_t *pd, void *vaddr, void *paddr, uint8_t flags);
static void pde_vaddr_to_paddr(uint32_t *pd);
static void copy_kernel_pd(uint32_t *pd, uint32_t *kernel_pd);

void switch_pd(uint32_t *pd) {
    set_pd((uint32_t)pd - KERNEL_START_VADDR);
}

uint32_t *create_user_pd() {
    uint32_t *pd = (uint32_t *)kcalloc(BLOCK_SIZE);

    // code and data space
    void *addr = kcalloc(BLOCK_SIZE) - KERNEL_START_VADDR;
    pd_mmap(pd, (void *)USER_CODE_VADDR, addr, 0x1f);

    // stack space
    addr = kcalloc(BLOCK_SIZE) - KERNEL_START_VADDR;
    pd_mmap(pd, (void *)USER_STACK_VADDR, addr, 0x1f);

    // change all virtual address in pde to physical address
    pde_vaddr_to_paddr(pd);

    copy_kernel_pd(pd, &kernel_pd);

    return pd;
}

/** pd_mmap  maps a physical address to a virtual address in pd
 *  so that the page directory contains that page frame.
 *  It creates a page table when necessary.
 *
 *  Note: pde stores the virtual address of page table and
 *  it shall be transferred to physical address by
 *  func pde_vaddr_to_paddr().
 */
static void pd_mmap(uint32_t *pd, void *vaddr, void *paddr, uint8_t flags) {
    size_t idx = (uint32_t)vaddr >> 22;
    // printf("%s: pd 0x%x, vaddr 0x%x, paddr 0x%x, flags %u\n", __func__, pd,
    //       vaddr, paddr, flags);
    if (pd[idx] == 0) {
        pd[idx] = ((uint32_t)kcalloc(BLOCK_SIZE) & 0xFFFFF000) | flags;
        //  printf("%s: add table pd[0x%x] = 0x%x\n", __func__, idx, pd[idx]);
    }

    uint32_t *table = (uint32_t *)(pd[idx] & 0xFFFFF000);
    idx = ((uint32_t)vaddr >> 12) % PT_SIZE;
    table[idx] = ((uint32_t)paddr & 0xFFFFF000) | flags;
}

static void pde_vaddr_to_paddr(uint32_t *pd) {
    size_t i = 0;
    for (; i < PD_SIZE; i++) {
        if (pd[i] != 0) {
            pd[i] -= KERNEL_START_VADDR;
        }
    }
}

static void copy_kernel_pd(uint32_t *pd, uint32_t *kernel_pd) {
    size_t i = 0;
    for (; i < PD_SIZE; i++) {
        if (kernel_pd[i] != 0) {
            pd[i] = kernel_pd[i];
        }
    }
}

static void page_fault(struct cpu_state, unsigned int,
                       struct stack_state stack);

void init_paging() {
    register_interrupt_handler(E_Page_Fault, page_fault);
}

static void page_fault(struct cpu_state, unsigned int,
                       struct stack_state stack) {
    uint32_t faulting_address;
    __asm__ volatile("mov %%cr2, %0" : "=r"(faulting_address));

    uint32_t ecode = stack.error_code;
    int present = !(ecode & 0x1); // Page not present
    int rw = ecode & 0x2;         // Write operation?
    int us = ecode & 0x4;         // Processor was in user-mode?
    int reserved = ecode & 0x8;   // Overwritten CPU-reserved bits of page entry

    // Output an error message.
    printf("Page fault! ( ");
    if (present) {
        printf("present ");
    }
    if (rw) {
        printf("read-only ");
    }
    if (us) {
        printf("user-mode ");
    }
    if (reserved) {
        printf("reserved ");
    }
    printf(") at 0x%08x\n", faulting_address);
    PANIC("Page fault");
}
