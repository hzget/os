Bare Bones
==========

It's a tutorial serving as an example of how to create a minimal system.
It will write a simple kernel for 32-bit x86 and boot it.

Its doc is here:
https://wiki.osdev.org/Bare_Bones

Structure
---------

* boot.s - kernel entry point that sets up the processor environment
* kernel.c - your actual kernel routines
* linker.ld - for linking the above files

Tools
-----

* bootloader: GRUB
* compiler: cross-compiler gcc
* emulator: qemu

