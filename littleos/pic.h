/** @file */

#define PIC_1 0x20 /* IO base address for master PIC */
#define PIC_2 0xA0 /* IO base address for slave PIC */
#define PIC_1_COMMAND PIC_1
#define PIC_1_DATA (PIC_1 + 1)
#define PIC_2_COMMAND PIC_2
#define PIC_2_DATA (PIC_2 + 1)

#define ICW1_ICW4 0x01      /* Indicates that ICW4 will be present */
#define ICW1_SINGLE 0x02    /* Single (cascade) mode */
#define ICW1_INTERVAL4 0x04 /* Call address interval 4 (8) */
#define ICW1_LEVEL 0x08     /* Level triggered (edge) mode */
#define ICW1_INIT 0x10      /* Initialization - required! */

#define ICW4_8086 0x01       /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO 0x02       /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE 0x08  /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C /* Buffered mode/master */
#define ICW4_SFNM 0x10       /* Special fully nested (not) */

#define PIC_1_START_INTERRUPT 0x20
#define PIC_2_START_INTERRUPT 0x28
#define PIC_2_END_INTERRUPT PIC_2_START_INTERRUPT + 7

#define PIC_ACK 0x20

/** pic_acknowledge:
 *  Acknowledges an interrupt from either PIC 1 or PIC 2.
 *
 *  @param num The number of the interrupt
 */
void pic_acknowledge(unsigned int interrupt);

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
void pic_reinitialize();
