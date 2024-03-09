/** @file */

#include "pic.h"
#include "io.h"

/** pic_acknowledge:
 *  Acknowledges an interrupt from either PIC 1 or PIC 2.
 *
 *  @param interrupt The number of the interrupt
 */
void pic_acknowledge(unsigned int interrupt) {
    if (interrupt < PIC_1_START_INTERRUPT || interrupt > PIC_2_END_INTERRUPT) {
        return;
    }

    if (interrupt >= PIC_2_START_INTERRUPT) {
        outb(PIC_2_COMMAND, PIC_ACK);
    }
    outb(PIC_1_COMMAND, PIC_ACK);
}

static void io_wait(void) {
    outb(0x80, 0);
}

/** pic_remap:
 *  Remap pic irq, giving them specified vector offsets.
 *
 *  In protected mode, the IRQs 0 to 7 conflict with the CPU exception which
 *  are reserved by Intel up until 0x1F. (It was an IBM design mistake.)
 *  Consequently it is difficult to tell the difference between an IRQ
 *  or an software error. It is thus recommended to change the PIC's offsets
 *  (also known as remapping the PIC) so that IRQs use non-reserved vectors.
 *
 *  A common choice is to move them to the beginning of the available
 *  range (IRQs 0..0xF -> INT 0x20..0x2F). For that, we need to set
 *  the master PIC's offset to 0x20 and the slave's to 0x28.
 *  For code examples, see below.
 *
 *  Default:
 *    - master pic IRQ 0 to 7  -----> cpu interrupt 0x08 to 0x0F (offset 0x08)
 *    - slave  pic IRQ 8 to 15 -----> cpu interrupt 0x70 to 0x77 (offset 0x70)
 *
 *  Remap:
 *    - master pic IRQ 0 to 7  -----> cpu interrupt 0x20 to 0x27 (offset 0x20)
 *    - slave  pic IRQ 8 to 15 -----> cpu interrupt 0x28 to 0x2F (offset 0x28)
 *
 *  @param offset1 cpu interrupt vector offset for master pic IRQ
 *  @param offset2 cpu interrupt vector offset for slave  pic IRQ
 */
static void pic_remap(int offset1, int offset2) {

    outb(PIC_1_DATA, offset1); // ICW2: Master PIC vector offset
    io_wait();
    outb(PIC_2_DATA, offset2); // ICW2: Slave PIC vector offset
    io_wait();
}

/** pic_reinitialize:
 *  Reinitialize the PIC controllers.
 *
 *  When you enter protected mode
 *  (or even before hand, if you're not using GRUB),
 *  the first command you will need to give the two PICs is the
 *  initialise command (code 0x11). This command makes the PIC
 *  wait for 3 extra "initialisation words" on the data port.
 *  These bytes give the PIC:
 *    - Its vector offset. (ICW2)
 *    - Tell it how it is wired to master/slaves. (ICW3)
 *    - Gives additional information about the environment. (ICW4)
 */
void pic_reinitialize() {

    // ICW1: starts the initialization sequence (in cascade mode)
    outb(PIC_1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC_2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    // ICW2: vector offset
    pic_remap(PIC_1_START_INTERRUPT, PIC_2_START_INTERRUPT);

    // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    outb(PIC_1_DATA, 4);
    io_wait();
    // ICW3: tell Slave PIC its cascade identity (0000 0010)
    outb(PIC_2_DATA, 2);
    io_wait();

    // ICW4: have the PICs use 8086 mode (and not 8080 mode)
    outb(PIC_1_DATA, ICW4_8086);
    io_wait();
    outb(PIC_2_DATA, ICW4_8086);
    io_wait();

    // Setup Interrupt Mask Register (IMR)
    // In our case, we will use timer/keyboard interrupt,
    // thus enable IRQ0 and IRQ1 - 1111 1100
    outb(PIC_1_DATA, 0xFC);
    outb(PIC_2_DATA, 0xFF);

    // Enable interrupts.
    __asm__ volatile("sti");
}
