Writing An Operating System From Scratch
========================================

This project intends to write an operating system from scratch
and demonstrates key points of each stage.
It's doc site is here:
[Operationg System](https://hzget.github.io/notes/os/).

Difference
----------

What's the difference from other tutorials of OS ?
***It is written from MY point of view***.

Besides, it just log my os learning tour.

Versions
--------

v1.0.0-alpha

 * bare - can run first kernel function
 * littleos - gdt, idt, framebuffer, serial

v1.0.1-alpha

 * littleos - support paging, memory management, user mode, system call, filesystem

Structure
---------

* [bare_bones](./bare_bones): a minimal system from osdev
* [bare](./bare): code on "bared" board before running an os
* [littleos](./littleos): relating to kernel & drivers
in [Little OS Book][littleosbook]
* [jamesm_os](./jamesm_os): an implementation of
[JamesM's kernel development tutorials][tutorial] (***Deprecated***)

Before Running An OS
--------------------

After POWERING ON the computer, the process is:

    BIOS --> MBR --> kernel loader --> kernel

BIOS will check hardware conditions, and then load MBR into memory,
run it.
MBR runs in REAL mode and loads the "kernel loader" into memory.
The "loader" switches to PROTECTED mode and loads kernel
into memory and run it. Now the Operating System (the kernel) is running.

[bare](./bare) focus on writing MBR and kernel loader that
run on the "bared" board. 

Little OS Book
--------------

[Littel OS Book][littleosbook] is a practical guide to writing
your own x86 operating system. It is designed to give enough help
with the technical details while at the same time not reveal
too much with samples and code excerpts.

It foucs on writing kernel and drivers. It just uses grub as
the bootloader.

Our repo [littleos](./littleos) will complete the code in this book.

[littleosbook]: https://littleosbook.github.io/
[tutorial]: http://www.jamesmolloy.co.uk/tutorial_html/
