/** @file */
#include "interrupts.h"
#include "framebuffer.h"
#include "keyboard.h"
#include "pic.h"

typedef struct {
    unsigned short isr_low;   // The lower 16 bits of the ISR's address
    unsigned short kernel_cs; // The GDT segment selector that the CPU will load
                              // into CS before calling the ISR
    unsigned char reserved;   // Set to zero
    unsigned char attributes; // Type and attributes; see the IDT page
    unsigned short isr_high;  // The higher 16 bits of the ISR's address
} __attribute__((packed)) idt_entry_t;

// Create an array of IDT entries; aligned for performance
__attribute__((aligned(0x10))) static idt_entry_t idt[256];

typedef struct {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed)) idtr_t;

static idtr_t idtr;

struct cpu_state {
    unsigned int eax;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;
    unsigned int esp;
    unsigned int ebp;
    unsigned int esi;
    unsigned int edi;
} __attribute__((packed));

struct stack_state {
    unsigned int error_code;
    unsigned int eip;
    unsigned int cs;
    unsigned int eflags;
} __attribute__((packed));

static void keyboard_interrupt_handler() {
    char key = read_keyboard_char();
    char buffer[1];
    buffer[0] = key;
    fb_write(buffer, sizeof(buffer));
}

/** interrupt_handler
 *  Handles interrupts.
 *
 *  @param cpu_state cpu registers that were saved in the stack
 *                   when an interrupt occurs
 *  @param interrupt num of this interrupt
 *  @param stack_state stack info that were saved in the stack
 *                   when an interrupt occurs
 */
void interrupt_handler(__attribute__((unused)) struct cpu_state,
                       unsigned int interrupt,
                       __attribute__((unused)) struct stack_state) {
    switch (interrupt) {
    case KEYBOARD_INTERRUPT:
        keyboard_interrupt_handler();
        pic_acknowledge(interrupt);
        break;
    default:
        pic_acknowledge(interrupt);
        break;
    }
}

static void idt_set_descriptor(unsigned char vector, void *isr,
                               unsigned char flags) {
    idt_entry_t *descriptor = &idt[vector];

    descriptor->isr_low = (unsigned int)isr & 0xFFFF;
    descriptor->kernel_cs = 0x08; // this value can be whatever offset your
                                  // kernel code selector is in your GDT
    descriptor->attributes = flags;
    descriptor->isr_high = (unsigned int)isr >> 16;
    descriptor->reserved = 0;
}

extern void *isr_stub_table[];

#define IDT_MAX_DESCRIPTORS 256

/** interrupts_install_idt:
 *  Install interrupt descriptor table to handle interrupt.
 *
 *  It does the following work:
 *      - reinitialize pic
 *      - create idt
 *      - load idt
 */
void interrupts_install_idt() {

    int i;
    for (i = 0; i < 35; i++) {
        idt_set_descriptor(i, isr_stub_table[i], 0x8E);
    }

    idtr.base = (unsigned int)&idt[0];
    idtr.limit = (unsigned short)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;
    __asm__ volatile("lidt %0" : : "m"(idtr)); // load the new IDT

    pic_reinitialize();
}
