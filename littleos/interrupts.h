/** @file */

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "pic.h"
#include <stdint.h>

// Exceptions: https://wiki.osdev.org/Exceptions
#define E_Page_Fault 14

// IRQ: https://wiki.osdev.org/Interrupts#Standard_ISA_IRQs
#define IRQ0 PIC_1_START_INTERRUPT // Programmable Interrupt Timer
#define IRQ1 IRQ0 + 1              // keyboard
#define IRQ2 IRQ0 + 2
#define IRQ3 IRQ0 + 3
#define IRQ4 IRQ0 + 4
#define IRQ5 IRQ0 + 5
#define IRQ6 IRQ0 + 6
#define IRQ7 IRQ0 + 7

#define INT_SYSCALL 128 // 0x80

struct interrupt_frame {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t reserved;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t interrupt;   // interrupt number
    uint32_t error_code;
    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;         // exist only for inter-level interrupt
    uint32_t ss;          // exist only for inter-level interrupt
} __attribute__((packed));

// Enables registration of callbacks for interrupts or IRQs.
// For IRQs, to ease confusion, use the #defines above as the
// first parameter.
typedef void *(*isr_t)(struct interrupt_frame *frame);
void register_interrupt_handler(uint8_t n, isr_t handler);

void init_idt();

#endif /* INTERRUPTS_H */
