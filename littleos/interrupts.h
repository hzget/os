/** @file */

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "pic.h"
#include "stdint.h"

#define IRQ0 PIC_1_START_INTERRUPT
#define IRQ1 IRQ0 + 1 // keyboard
#define IRQ2 IRQ0 + 2
#define IRQ3 IRQ0 + 3
#define IRQ4 IRQ0 + 4
#define IRQ5 IRQ0 + 5
#define IRQ6 IRQ0 + 6
#define IRQ7 IRQ0 + 7

struct cpu_state {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
} __attribute__((packed));

struct stack_state {
    uint32_t error_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
} __attribute__((packed));

// Enables registration of callbacks for interrupts or IRQs.
// For IRQs, to ease confusion, use the #defines above as the
// first parameter.
typedef void (*isr_t)(struct cpu_state, uint32_t interrupt, struct stack_state);
void register_interrupt_handler(uint8_t n, isr_t handler);

void interrupts_install_idt();

#endif /* INTERRUPTS_H */
