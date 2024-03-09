#include "pit.h"
#include "io.h"
#include "task.h"

void timer_phase(int hz);
static void *pit_handler(struct interrupt_frame *frame);

void init_pit() {
    timer_phase(100);
    register_interrupt_handler(IRQ0, pit_handler);
}

static void *pit_handler(struct interrupt_frame *frame) {
    pic_acknowledge(frame->interrupt);
    task_next();
    return 0;
}

void timer_phase(int hz) {
    int divisor = 1193180 / hz; /* Calculate our divisor */
    outb(0x43, 0x36);           /* Set our command byte 0x36 */
    outb(0x40, divisor & 0xFF); /* Set low byte of divisor */
    outb(0x40, divisor >> 8);   /* Set high byte of divisor */
}
