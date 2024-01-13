#include "paging.h"
#include "interrupts.h"
#include "stdio.h"

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
