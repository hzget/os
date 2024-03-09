#ifndef TASK_H
#define TASK_H

#include "interrupts.h"
#include "process.h"
#include <stdint.h>

struct registers {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;

    uint32_t ip;
    uint32_t cs;
    uint32_t flags;
    uint32_t esp;
    uint32_t ss;
};

struct task {
    uint32_t *page_directory;
    struct registers registers;
    struct task *next;
    struct task *prev;
    struct process *process;
    int mmapped;
};

struct process;

struct task *task_new(struct process *process);
struct task *task_current();
int task_free(struct task *task);
int task_switch(struct task *task);

void task_next();
void task_return(struct registers *regs);
void restore_general_purpose_registers(struct registers *regs);
void user_registers();

void task_run_first_ever_task();

void task_current_save_state(struct interrupt_frame *frame);
void *task_get_stack_item(struct task *task, int index);
#endif
