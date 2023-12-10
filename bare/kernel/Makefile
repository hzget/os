
image:
	nasm -f bin mbr.asm -o boot.bin
	nasm -f bin loader.asm -o loader.bin

	nasm -f elf32 start.asm -o start.o
	gcc -m32 -S main.c -o main.s
	as -32 main.s -o main.o
	ld -m elf_i386 -static start.o main.o -o kernel.bin -Ttext 0x10000
	objcopy -O binary kernel.bin system.bin

	bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat master.img
	dd if=boot.bin of=master.img conv=notrunc
	dd if=loader.bin of=master.img seek=2 conv=notrunc
	dd if=system.bin of=master.img seek=10 count=100 conv=notrunc

clean:
	rm *.s *.o *.bin *.img
