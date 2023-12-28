Little OS
=========

[Littel OS Book][littleosbook] is a practical guide to writing
your own x86 operating system. It is designed to give enough help
with the technical details while at the same time not reveal
too much with samples and code excerpts.

It foucs on writing kernel and drivers. It just uses grub as
the bootloader.

Our repo will complete the code in this book.

structure
---------

[loader.s](./loader.s) is the entrance of the kernel  
[framebuffer.h](./framebuffer.h), [io.s](./io.s) an output driver
to write to framebuffer  
[serial.h](./serial.h) an serial driver to write to a serial port  
[segments.h](./segments.h), [gdt.s](./gdt.s) install a new gdt  
[interrupts.h](./interrupts.h), [pic.h](./pic.h),
[interrupt_handlers.s](./interrupt_handlers.s),
[keyboard.h](./keyboard.h) install a new idt to handle keyboard
interrupt  

drivers
-------

A driver acts as a layer between kernel and hardware,
providing a higher abstraction than communicating directly
with the hardware. For example, output driver
hides the detail hardware operation and provide
a `write` function with the following declaration:

```bash
int fb_write(char *buf, unsigned int len);
```

The kernel can use it directly to write a buffer to the framebuffer.
Detailed hareware operations are implemented in
[io.s](./io.s) and [framebuffer.c](./framebuffer.c).

Interrupt
---------

keyword:

 * Interrupt Descriptor Table (IDT)
 * Interrupt Service Routine (ISR)
 * Programmable Interrupt Controller (PIC)

Interrupts are signals from a device, such as a keyboard or a hard drive,
to the CPU, telling it to immediately stop whatever it is currently doing
and do something else.
The CPU looks up corresponding entry in the Interrupt Descriptor Table
(IDT), and jump to run the code the entry points to.
After that, the CPU returns to its previous tasks.

The code that is run in response to the interrupt is known as
a interrupt service routine (ISR) or an interrupt handler.

[littleosbook][littleosbook] gives a simple and clear explanation
of the interrupt handling machnism.  
[interrupts][interrupts] gives an overall introduction and gives
details from every participant's views.  
[8259 PIC][8259 PIC] gives introduction and example codes of PIC  
[Interrupts tutorial][Interrupts tutorial] gives example codes
of how to create and install IDT.

[Interrupts tutorial]: https://wiki.osdev.org/Interrupts_tutorial
[8259 PIC]: https://wiki.osdev.org/8259_PIC
[interrupts]: https://wiki.osdev.org/Interrupts
[littleosbook]: https://littleosbook.github.io/
