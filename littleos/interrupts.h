/** @file */

// Wrappers around ASM.
void load_idt(unsigned int idt_address);
void isr_stub_33();

void interrupts_install_idt();
