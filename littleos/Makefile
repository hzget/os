#
# cross-compiler:
#
#     https://wiki.osdev.org/GCC_Cross-Compiler
#
#
.PHONY: test

OBJECTS = loader.o kmain.o framebuffer.o io.o serial.o segments.o gdt.o interrupts.o interrupt_handlers.o pic.o keyboard.o log.o
OBJECTS += stdio.o check.o paging.o module.o kheap.o mem.o string.o pd.o user_mode.o
OBJECTS += tss.o syscall.o tss_asm.o
OBJECTS += disk/disk.o disk/streamer.o fs/pparser.o fs/file.o fs/fat/fat16.o
OBJECTS += task/task.o task/process.o task/task_asm.o pit.o

AS = nasm
#AS = i686-elf-as
CC = i686-elf-gcc
LD = $(CC)

ASFLAGS = -f elf
CFLAGS = -std=gnu99 -ffreestanding -O0 -Wall -Wextra -Werror -c -g
CFLAGS += -pedantic
CFLAGS += -I. -I./disk -I./fs -I./fs/fat -I./task
LDFLAGS = -T link.ld -ffreestanding -O0 -nostdlib -lgcc

AS_HEADERS = constants.inc

ISO_BIN_PATH = iso/modules/
ISO_APPS = main

BIN_FOLDER = bin
HDDIR = harddisk
HD = $(HDDIR)/harddisk.img

ISO_DIR = iso

all: os.iso

build: kernel.elf build_iso_apps

kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o kernel.elf

os.iso: kernel.elf create_iso_bin_folder create_harddisk_image

	mkdir -p $(ISO_DIR)/boot/grub
	cp kernel.elf $(ISO_DIR)/boot/kernel.elf
	cp grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o os.iso $(ISO_DIR)

create_harddisk_image: copy_binaries
	@echo '-> Creating harddisk image';
	@make --no-print-directory -C $(HDDIR);
	@echo ''; \

copy_binaries: build_programs
	@echo '-> Copying applications to bin folder'
	if ! [ -d $(BIN_FOLDER) ]; then mkdir -p $(BIN_FOLDER); fi
	@make --no-print-directory -C programs copy_binaries
	@echo ''

build_programs:
	@echo '-> Building programs'
	@make --no-print-directory -C programs
	@echo ''

create_iso_bin_folder: build_iso_apps
	@echo '-> Copying iso applications to iso_bin folder'
	if ! [ -d $(ISO_BIN_PATH) ]; then mkdir -p $(ISO_BIN_PATH); fi
	for app in $(ISO_APPS); do cp iso_apps/$$app $(ISO_BIN_PATH); done
	@echo ''

build_iso_apps:
	@echo '-> Building iso applications'
	@make --no-print-directory -C iso_apps
	@echo ''

bochs: os.iso
	bochs -q

qemu: os.iso
#	qemu-system-x86_64 -boot d -cdrom os.iso -m 4 -monitor stdio
	qemu-system-i386 -boot d -cdrom os.iso -hdb $(HD) -m 256M -serial file:com1.qemu.out
qemugdb: os.iso
	#qemu-system-i386 -s -S -m 32M -boot c -hda master.img
	qemu-system-i386 -s -S -m 256M -boot d -cdrom os.iso -hdb $(HD) -serial file:com1.qemu.out


%.o: %.c
	$(CC) $(CFLAGS)  $< -o $@

%.o: %.s
	./c_to_nasm.sh $(AS_HEADERS)
	$(AS) $(ASFLAGS) $< -o $@

test:
	@echo '-> unit test'
	@make --no-print-directory -C test all

clean:
	@echo '-> Cleaning iso_apps'
	@make --no-print-directory -C iso_apps clean
	@echo ''
	@echo '-> Cleaning programs'
	@make --no-print-directory -C programs clean
	@echo ''
	@echo '-> Cleaning harddisk'
	@make --no-print-directory -C $(HDDIR) clean
	@echo ''
	@echo '-> Cleaning test'
	@make --no-print-directory -C test clean
	@echo ''
	@echo '-> Cleaning iso_bin'
	if [ -d $(ISO_BIN_PATH) ]; then rm $(ISO_BIN_PATH) -rf; fi
	@echo ''
	@echo '-> Cleaning bin'
	if [ -d $(BIN_FOLDER) ]; then rm $(BIN_FOLDER) -rf; fi
	@echo ''
	rm -rf *.o kernel.elf os.iso *.out $(ISO_DIR) *.img
	find . -name '*.o' -type f -delete

clang-format:
	find . -type f \( -name '*.c' -o -name '*.h' \) -exec clang-format -i {} +

