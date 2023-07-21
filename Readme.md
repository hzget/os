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

Structure
---------

* before running an os
* kernel

Before Running An OS
--------------------

After POWERING ON the computer, the process is:

    BIOS --> MBR --> kernel loader --> kernel

BIOS will check hardware conditions, and then load MBR into memory,
run it.
MBR runs in REAL mode and loads the "kernel loader" into memory.
The "loader" switches to PROTECTED mode and loads kernel
into memory and run it. Now the Operating System (the kernel) is running.

codes:

[printString.asm](./bare/printString.asm) - MBR prints a string
"MBR is running" via BIOS interrupt

[graphics.asm](./bare/graphics.asm) - MBR prints a tring
"2 MBR" directly to the memory of Graphics Card

[harddisk.asm](./bare/harddisk.asm) - MBR loads a "loader" from
harddisk to the memory and then runs it

[toyloader.asm](./bare/toyloader.asm) - a toy loader loaded by
a MBR program
