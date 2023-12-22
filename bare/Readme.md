Before Running An OS
====================

After POWERING ON the computer, the process is:

    BIOS --> MBR --> kernel loader --> kernel

BIOS will check hardware conditions, and then load MBR into memory,
run it.
MBR runs in REAL mode and loads the "kernel loader" into memory.
The "loader" switches to PROTECTED mode and loads kernel
into memory and run it. Now the Operating System (the kernel) is running.

Our **focus** is to write code that runs on the bared board.
We will write all the things from scratch. It ends with kernel
running its 1st function.
All other things relating to the kernel are talked
in the other directory.

codes (env to run the code [ENV][bochs env]):

[printString.asm](./printString.asm) - MBR prints a string
"MBR is running" via BIOS interrupt

[graphics.asm](./graphics.asm) - MBR prints a string
"2 MBR" directly to the memory of Graphics Card

[harddisk.asm](./harddisk.asm) - MBR loads a "loader" from
harddisk to the memory and then runs it

[toyloader.asm](./toyloader.asm) - a toy loader loaded by
a MBR program

[pmode.asm](./pmode.asm) - MBR prints a string in realmode then
switches to protected mode and prints a string in that mode

[enter kernel](./kernel/) - shows how to run into kernel code

[bochs env]: https://hzget.github.io/notes/os/bare/env.md

