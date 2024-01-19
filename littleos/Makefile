OBJECTS = loader.o kmain.o framebuffer.o io.o serial.o segments.o gdt.o interrupts.o interrupt_handlers.o pic.o keyboard.o
OBJECTS += stdio.o check.o paging.o module.o kheap.o mem.o string.o pd.o user_mode.o
CC = gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
	-nostartfiles -nodefaultlibs -Wall -Wextra -Werror -g -c
LDFLAGS = -T link.ld -melf_i386
AS = nasm
ASFLAGS = -f elf
AS_HEADERS = constants.inc

BIN_PATH = iso/modules/
APPS = main

ISO_DIR = iso

all: os.iso

kernel.elf: $(OBJECTS)
	ld $(LDFLAGS) $(OBJECTS) -o kernel.elf

os.iso: kernel.elf create_bin_folder

	mkdir -p $(ISO_DIR)/boot/grub
	cp kernel.elf $(ISO_DIR)/boot/kernel.elf
	cp grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o os.iso $(ISO_DIR)

create_bin_folder: build_apps
	@echo '-> Copying applications to bin folder'
	if ! [ -d $(BIN_PATH) ]; then mkdir -p $(BIN_PATH); fi
	for app in $(APPS); do cp apps/$$app $(BIN_PATH); done
	@echo ''

build_apps:
	@echo '-> Building applications'
	@make --no-print-directory -C apps
	@echo ''

bochs: os.iso
	bochs -q

qemu: os.iso
#	qemu-system-x86_64 -boot d -cdrom os.iso -m 4 -monitor stdio
	qemu-system-i386 -boot d -cdrom os.iso -m 256M -serial file:com1.qemu.out
qemugdb: os.iso
	#qemu-system-i386 -s -S -m 32M -boot c -hda master.img
	qemu-system-i386 -s -S -m 256M -boot d -cdrom os.iso

%.o: %.c
	$(CC) $(CFLAGS)  $< -o $@

%.o: %.s
	./c_to_nasm.sh $(AS_HEADERS)
	$(AS) $(ASFLAGS) $< -o $@

clean:
	@echo '-> Cleaning apps'
	@make --no-print-directory -C apps clean
	@echo ''
	@echo '-> Cleaning bin'
	if [ -d $(BIN_PATH) ]; then rm $(BIN_PATH) -rf; fi
	rm -rf *.o kernel.elf os.iso *.out $(ISO_DIR)

clang-format:
	clang-format -i *.c *.h

