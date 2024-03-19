#include "syscall.h"
#include "config.h"
#include "interrupts.h"
#include "stdio.h"
#include "task.h"

void *isr80h_command0_sum(struct interrupt_frame *frame);
void *isr80h_command1_print(struct interrupt_frame *frame);

static ISR80H_COMMAND isr80h_commands[MAX_ISR80H_COMMANDS];

void init_syscalls() {
    isr80h_register_command(SYSTEM_COMMAND0_SUM, isr80h_command0_sum);
    isr80h_register_command(SYSTEM_COMMAND1_PRINT, isr80h_command1_print);
    register_interrupt_handler(INT_SYSCALL, isr80h_handler);
}

void *isr80h_command0_sum(struct interrupt_frame *frame) {
    (void)frame;
    int v2 = (int)task_get_stack_item(task_current(), 1);
    int v1 = (int)task_get_stack_item(task_current(), 0);
    return (void *)(v1 + v2);
}

void *isr80h_command1_print(struct interrupt_frame *frame) {
    (void)frame;
    struct task *task = task_current();
    char *message = (char *)task_get_stack_item(task, 0);
    printf("%s: %s\n", task->process->filename, message);

    return 0;
}

void isr80h_register_command(int command_id, ISR80H_COMMAND command) {
    if (command_id < 0 || command_id >= MAX_ISR80H_COMMANDS) {
        PANIC("The command is out of bounds\n");
    }

    if (isr80h_commands[command_id]) {
        PANIC("Your attempting to overwrite an existing command\n");
    }

    isr80h_commands[command_id] = command;
}

void *isr80h_handle_command(struct interrupt_frame *frame) {
    int command_id = (int)frame->eax;

    if (command_id < 0 || command_id >= MAX_ISR80H_COMMANDS) {
        return 0;
    }

    ISR80H_COMMAND command_func = isr80h_commands[command_id];
    if (!command_func) {
        return 0;
    }

    return command_func(frame);
}

void *isr80h_handler(struct interrupt_frame *frame) {
    task_current_save_state(frame);
    return isr80h_handle_command(frame);
}

