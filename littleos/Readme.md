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
* [x] System Calls
* [x] File Systems
* [ ] Multitasking
* [ ] add gcc options -Wconversion to avoid unexpected behavior
* [ ] unit test

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

- [x] serial
- [x] keyboard
- [x] vga
- [x] harddisk
- [x] filesystem

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

Global Descriptor Table (GDT)
-----------------------------

The Global Descriptor Table (GDT) is a binary data structure specific to
the IA-32 and x86-64 architectures.
It contains entries telling the CPU about memory segments.
Each time the CPU wants to access some memory data (such as
to get next code instruction), it will
refer to GDT to get corresponding info such as
segment address, descriptor privilege level (DPL) and so forth.
Then it will:

* perform safety and access control checks
* cache useful values in (in)visible CPU registers.
* access the required memory data

Next time when the CPU wants to access the same segement,
it will just use the cached values in the registers.

***Structure of the code***:  
[segments.h](./segments.h) provides funcs to construct GDT  
[gdt.s](./gdt.s) provides funcs to load GDT info to processor  

***Reference***:  
[Global Descriptor Table][GDT],  
[GDT Tutorial][GDT Tutorial],  
[Global Descriptor Table (wikipedia)][GDT (wikipedia)],  

Interrupt Descriptor Table (IDT)
--------------------------------

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

***Structure of the code***:  
[interrupts.h](./interrupts.h) provides the ***IDT*** initializer
and an interface to register handlers  
[interrupt_handlers.s](./interrupt_handler.s) provides a common handler
that prepares stack for specific handlers.

***References***:  
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

File System
-----------

The kernel provides a file system so that the users can
read/write files easily without having to interact with
the hardware.

To read/write data stored on the harddisk, the kernel
has to read/write corresponding sectors. These operations
shall follow corresponding specifications. In our case,
we use ATA specification.

File data structure in the hardware corresponds to
a specific filesystem format such as FAT16, FAT32, NTFS
and so forth. For our case, we use FAT16.

The kernel abstracts all the functions and provides
fopen, fread, fstat and fclose utilities for the user.

```bash
Structure of FileSystem

                            ----------      ----------               ------------
                            |        |      |        |     ATA       |          |
                       -----| FAT32  | ---- |  data  |  ------------ | Harddisk |
                       |    | driver |      | stream | specification |   0:/    |
                       |    |        |      |        |               |          |
                       |    ----------      ----------               ------------
                       |
          -----------  |    ----------      ----------               ------------
          | Virtual |  |    |        |      |        |      ATA      |          |
user ---- | File    |-------| FAT16  | ---- |  data  | ------------- | Harddisk |
          | System  |       | driver |      | stream | specification |   1:/    |
          | (VFS)   |       |        |      |        |               |          |
          -----------       ----------      ----------               ------------
            fopen,          fat16_open,   diskstream_new           inb/inw/outb/outw
            fseek,          fat16_seek,   diskstream_seek
            fread,          fat16_read,   diskstream_read
            fclose,         fat16_close,  diskstream_close
            fstat,          fat16_stat,
```

***Structure of the code***

 * [fs/file.h](./fs/file.h) contains VFS interfaces
 * [fs/fat/fat16.h](./fs/fat/fat16.h) contains fat16 drivers
 * [disk/streamer.h](./disk/streamer.h) contains harddisk data stream
 * [disk/disk.h](./disk/disk.h) contains disk operations that take a sector as a unit
 * [io.h](./io.h) contains in/out funcs to access harddisk via ATA specification

References:  
http://www.maverick-os.dk/FileSystemFormats/FAT16_FileSystem.html  
https://wiki.osdev.org/FAT  
https://github.com/nibblebits/PeachOS  

Unit Test
---------

Currently support to run a unit test with ***cmocka***.
For example:

```bash
# make test
> unit test
rm *_test *.o *.so -rf
gcc -c -fPIC -I../ ../string.c -o string.o -fno-builtin
gcc -shared -Wl,-soname,libstring.so -o libstring.so string.o
gcc -o string_test string_test.c -L. -Wl,-R. -lstring -lcmocka -DUNIT_TESTING=1 -fno-builtin
./string_test
[==========] Running 1 test(s).
[ RUN      ] test_strlen
[       OK ] test_strlen
[==========] 1 test(s) run.
[  PASSED  ] 1 test(s).
```

[higher half]: https://wiki.osdev.org/Higher_Half_bare_bones
[paging]: https://wiki.osdev.org/Paging
[Interrupts tutorial]: https://wiki.osdev.org/Interrupts_tutorial
[8259 PIC]: https://wiki.osdev.org/8259_PIC
[interrupts]: https://wiki.osdev.org/Interrupts
[littleosbook]: https://littleosbook.github.io/
[GDT]: https://wiki.osdev.org/Global_Descriptor_Table
[GDT Tutorial]: https://wiki.osdev.org/GDT_Tutorial
[GDT (wikipedia)]: https://en.wikipedia.org/wiki/Global_Descriptor_Table
