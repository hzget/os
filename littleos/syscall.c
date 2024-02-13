#include "syscall.h"
#include "interrupts.h"
#include "stdio.h"

static void syscall_handler(struct cpu_state, uint32_t,
                            struct stack_state stack);

void init_syscalls() {
    register_interrupt_handler(INT_SYSCALL, syscall_handler);
}

static void syscall_handler(struct cpu_state cpu, uint32_t interrupt,
                            struct stack_state stack) {
    (void)cpu;
    (void)interrupt;
    (void)stack;
    printf("%s is triggered\n", __func__);
    asm volatile("xchgw %bx, %bx");
}
