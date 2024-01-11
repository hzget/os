//
// isr.c -- High level interrupt service routines and interrupt request
// handlers. Part of this code is modified from Bran's kernel development
// tutorials. Rewritten for JamesM's kernel development tutorials.
//

#include "isr.h"
#include "common.h"
#include "stdio.h"

// This gets called from our ASM interrupt handler stub.
void isr_handler(registers_t regs) {
    printf("recieved interrupt: %u\n", regs.int_no);
}