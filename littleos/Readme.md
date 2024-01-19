Little OS
=========

[Littel OS Book][littleosbook] is a practical guide to writing
your own x86 operating system. It is designed to give enough help
with the technical details while at the same time not reveal
too much with samples and code excerpts.

It foucs on writing kernel and drivers. It just uses grub as
the bootloader.

Our repo will complete the code in this book.

* [x] IO drivers: searial/vga output, keyboard input
* [x] Global Descriptor Table (GDT)
* [x] Interrupt Descriptor Table (IDT)
* [x] Paging
* [x] Memory Allocation: malloc/calloc/free
* [x] User mode
* [ ] System Calls
* [ ] File Systems
* [ ] Multitasking

structure
---------

[loader.s](./loader.s) is the entrance of the kernel  
[kmain.c](./kmain.c) contains the main function  
[framebuffer.h](./framebuffer.h), [io.s](./io.s) an output driver
to write to framebuffer  
[serial.h](./serial.h) an serial driver to write to a serial port  
[segments.h](./segments.h), [gdt.s](./gdt.s) install a new
global descriptor table  
[interrupts.h](./interrupts.h), [pic.h](./pic.h),
[interrupt_handlers.s](./interrupt_handlers.s),
[keyboard.h](./keyboard.h) install a new idt to handle keyboard
interrupt  
[kheap.h](./kheap.h) allocate a space (a frame as unit) for use  
[paging.h](./paging.h) create user page directory and switch pd  

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

Paging
------

Paging is used to enable virtual memory. Virtual memory through paging
means that each process will get the impression that the available memory
range is 0x00000000 - 0xFFFFFFFF even though the actual size of the memory
might be much less.

When a process addresses a byte of memory it will use a virtual address
instead of physical one.
The virtual address gets translated to a physical address by the MMU and
the page table.
To make it work, we shall create a table decribing mapping rules for them.

Note that user process memory starts from the beginning of the memory
space, thus the kernel shall use other spaces. And we usually use
higher part of the space for the kernel.

```bash 
kernel address space:
virtual address  0xC0100000 ~ (0xC0100000+kernelsize) ----->  
physical address 0x00100000 ~ kernelsize

1M space before kernel:
(BIOS and GRUB code loaded below 1 MB)
virtual address  0xC0000000 ~ 0xC0100000 ----->  
physical address 0x00000000 ~ 0x00100000
```

Paging in x86 consists of a page directory (PDT) that can contain
references to 1024 page tables (PT), each of which can point to
1024 sections of **physical memory** called page frames (PF).
Each page frame is 4096 byte large.

[littelosbook][littleosbook] gives a simple and clear explanation
of paging mechnism.  
[paging][paging] gives details of PDT, PT, MMU  
[higher half][higher half] gives a tutorial for making a higher-half
kernel  

[higher half]: https://wiki.osdev.org/Higher_Half_bare_bones
[paging]: https://wiki.osdev.org/Paging
[Interrupts tutorial]: https://wiki.osdev.org/Interrupts_tutorial
[8259 PIC]: https://wiki.osdev.org/8259_PIC
[interrupts]: https://wiki.osdev.org/Interrupts
[littleosbook]: https://littleosbook.github.io/
