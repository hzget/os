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


[littleosbook]: https://littleosbook.github.io/
