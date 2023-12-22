OBJECTS = loader.o kmain.o framebuffer.o io.o
CC = gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
	-nostartfiles -nodefaultlibs -Wall -Wextra -Werror -g -c
LDFLAGS = -T link.ld -melf_i386
AS = nasm
ASFLAGS = -f elf

all: kernel.elf

kernel.elf: $(OBJECTS)
	ld $(LDFLAGS) $(OBJECTS) -o kernel.elf

os.iso: kernel.elf
	cp kernel.elf iso/boot/kernel.elf
	genisoimage -R                              \
                    -b boot/grub/stage2_eltorito    \
                    -no-emul-boot                   \
                    -boot-load-size 4               \
                    -A os                           \
                    -input-charset utf8             \
                    -quiet                          \
                    -boot-info-table                \
                    -o os.iso                       \
                    iso

bochs: os.iso
	bochs -q

qemu: os.iso
#	qemu-system-x86_64 -boot d -cdrom os.iso -m 4 -monitor stdio
	qemu-system-i386 -boot d -cdrom os.iso -m 4
qemugdb: os.iso
	#qemu-system-i386 -s -S -m 32M -boot c -hda master.img
	qemu-system-i386 -s -S -m 4 -boot d -cdrom os.iso

%.o: %.c
	$(CC) $(CFLAGS)  $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf *.o kernel.elf os.iso

