#ifndef SYSCALL_H
#define SYSCALL_H
#include "interrupts.h"
#include <stdint.h>

enum SystemCommands {
    SYSTEM_COMMAND0_SUM,
    SYSTEM_COMMAND1_PRINT,
};

void init_syscalls();

typedef void *(*ISR80H_COMMAND)(struct interrupt_frame *frame);
void isr80h_register_command(int command_id, ISR80H_COMMAND command);
void *isr80h_handler(struct interrupt_frame *frame);

#endif
