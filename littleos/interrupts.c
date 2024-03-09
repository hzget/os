/** @file */
#include "interrupts.h"
#include "keyboard.h"
#include "pic.h"
#include "stdio.h"

typedef struct {
    uint16_t isr_low;   // The lower 16 bits of the ISR's address
    uint16_t kernel_cs; // The GDT segment selector that the CPU will load
                        // into CS before calling the ISR
    uint8_t reserved;   // Set to zero
    uint8_t attributes; // Type and attributes; see the IDT page
    uint16_t isr_high;  // The higher 16 bits of the ISR's address
} __attribute__((packed)) idt_entry_t;

// Create an array of IDT entries; aligned for performance
__attribute__((aligned(0x10))) static idt_entry_t idt[256];

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idtr_t;

static idtr_t idtr;

isr_t interrupt_handlers[256];

void register_interrupt_handler(uint8_t n, isr_t handler) {
    interrupt_handlers[n] = handler;
}

/** interrupt_handler
 *  Handles interrupts.
 */
void *interrupt_handler(struct interrupt_frame *frame) {
    void *res = 0;
    isr_t handler = interrupt_handlers[frame->interrupt];
    if (handler) {
        res = handler(frame);
    }
    pic_acknowledge(frame->interrupt);
    return res;
}

static void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags) {
    idt_entry_t *descriptor = &idt[vector];

    descriptor->isr_low = (uint32_t)isr & 0xFFFF;
    descriptor->kernel_cs = 0x08; // this value can be whatever offset your
                                  // kernel code selector is in your GDT
    descriptor->attributes = flags;
    descriptor->isr_high = (uint32_t)isr >> 16;
    descriptor->reserved = 0;
}

extern void *isr_stub_table[];

#define IDT_MAX_DESCRIPTORS 256

/** init_idt:
 *  Install interrupt descriptor table to handle interrupt.
 *
 *  It does the following work:
 *      - reinitialize pic
 *      - create idt
 *      - load idt
 */
void init_idt() {

    int i;
    for (i = 0; i < 34; i++) {
        idt_set_descriptor(i, isr_stub_table[i], 0x8E);
    }

    // for system call
    idt_set_descriptor(INT_SYSCALL, isr_stub_table[i], 0xEE);

    idtr.base = (uint32_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;
    __asm__ volatile("lidt %0" : : "m"(idtr)); // load the new IDT

    pic_reinitialize();
    printf("idt is installed\n");
}
