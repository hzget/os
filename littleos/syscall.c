#include "syscall.h"
#include "interrupts.h"
#include "stdio.h"

static void syscall_handler(struct cpu_state, uint32_t,
                            struct stack_state stack);

void init_syscalls() {
    register_interrupt_handler(INT_SYSCALL, syscall_handler);
}

static void syscall_handler(struct cpu_state, uint32_t, struct stack_state) {
    printf("%s is triggered\n", __func__);
    asm volatile("xchgw %bx, %bx");
}
