# Makefile
#
# Note:
#
#   Memory Space
#
#   mbr    0x7c00  -- real mode address
#   loader 0x1000  -- real mode address
#   kernel 0x10000 -- protected mode address
#
#   Linker:
#
#   The loader will load kernel image to 0x10000
#   and then jump there to run the kernel code.
#   Thus the linker should set that text setion begins from 0x10000.
#   It just uses `-Ttext 0x10000` for that.
#
#   **!!!Attention!!!**
#   the order of the `*.o` files in the ld command is very important.
#   start.o should be the first so that its code is in the beginning
#   of the text section. It in turn makes sure that `_start` in
#   start.asm will be in the address of 0x10000 which is expected.
#
#   qemu gdb:
#
#   qemu will start a gdb server and listen to port "1234"
#   any gdb client can connect to it this way:
#   # gdb
#   (gdb) target remote <server-ip-address>:1234
#   (gdb) file kernel.bin
#
image:
	nasm -f bin mbr.asm -o boot.bin
	nasm -f bin loader.asm -o loader.bin

	nasm -f elf32 start.asm -o start.o
	gcc -m32 -g -S main.c -o main.s
	as -32 main.s -o main.o
	ld -m elf_i386 -static start.o main.o -o kernel.bin -Ttext 0x10000
	objcopy -O binary kernel.bin system.bin

	bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat master.img
	dd if=boot.bin of=master.img conv=notrunc
	dd if=loader.bin of=master.img seek=2 conv=notrunc
	dd if=system.bin of=master.img seek=10 count=100 conv=notrunc

clean:
	rm *.s *.o *.bin *.img

bochs:
	bochs -q

qemu:
	qemu-system-i386 -m 32M -boot c -hda master.img
qemugdb:
	qemu-system-i386 -s -S -m 32M -boot c -hda master.img
