
OBJECTS = kernel.o boot.o

AS = i686-elf-as
CC = i686-elf-gcc
LD = $(CC)

ASFLAGS =
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -c
LDFLAGS = -T linker.ld -ffreestanding -O2 -nostdlib -lgcc

ISO_DIR = isodir

all: os.iso

build: kernel.elf
run: qemu

kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o kernel.elf

os.iso: kernel.elf
	mkdir -p $(ISO_DIR)/boot/grub
	cp kernel.elf $(ISO_DIR)/boot/kernel.elf
	cp grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o os.iso $(ISO_DIR)

qemu: os.iso
	qemu-system-i386 -cdrom os.iso

qemugdb: os.iso
	qemu-system-i386 -s -S -cdrom os.iso

%.o: %.c
	$(CC) $(CFLAGS)  $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf *.o *.elf *.iso *.out $(ISO_DIR)

