Enter Kernel
============

After POWERING ON the computer, the process is:

    BIOS --> MBR --> kernel loader --> kernel

loader will change from real mode to protected mode
and then jump to run kernel code.

Memory Space
-------------

mbr    0x7c00  -- real mode address  
loader 0x1000  -- real mode address  
kernel 0x10000 -- protected mode address

instructions
------------

first build the [Environment](https://hzget.github.io/notes/os/bare/env.md)

usage:

* make		- make image file
* make bochs	- start bochs to run the system
* make qemu	- start qemu to run the system
* make qemugdb	- start qemu and gdb to debug

Please refer to Makefile for details.

