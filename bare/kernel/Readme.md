Enter Kernel
============

After POWERING ON the computer, the process is:

    BIOS --> MBR --> kernel loader --> kernel

loader will change from real mode to protected mode
and then jump to run kernel code.

instructions
------------

first build the [Environment](https://hzget.github.io/notes/os/bare/env.md)

then run the following

```bash
nasm -f bin mbr.asm -o boot.bin
nasm -f bin loader.asm -o loader.bin

nasm -f elf32 start.asm -o start.o
gcc -m32 -S main.c -o main.s
as -32 main.s -o main.o
ld -m elf_i386 -static start.o main.o -o kernel.bin -Ttext 0x10000
objcopy -O binary kernel.bin system.bin

dd if=boot.bin of=master.img
dd if=loader.bin of=master.img seek=2
dd if=system.bin of=master.img seek=10 count=100

#run bochs
bochsdbg -q
```

