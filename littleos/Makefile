#
# cross-compiler:
#
#     https://wiki.osdev.org/GCC_Cross-Compiler
#
#
OBJECTS = loader.o kmain.o framebuffer.o io.o serial.o segments.o gdt.o interrupts.o interrupt_handlers.o pic.o keyboard.o
OBJECTS += stdio.o check.o paging.o module.o kheap.o mem.o string.o pd.o user_mode.o
OBJECTS += tss.o syscall.o tss_asm.o
OBJECTS += disk/disk.o disk/streamer.o fs/pparser.o fs/file.o fs/fat/fat16.o

AS = nasm
#AS = i686-elf-as
CC = i686-elf-gcc
LD = $(CC)

ASFLAGS = -f elf
CFLAGS = -std=gnu99 -ffreestanding -O0 -Wall -Wextra -Werror -c -g
CFLAGS += -I. -I./disk -I./fs -I./fs/fat
LDFLAGS = -T link.ld -ffreestanding -O0 -nostdlib -lgcc

AS_HEADERS = constants.inc

BIN_PATH = iso/modules/
APPS = main
HDDIR = harddisk
HD = $(HDDIR)/harddisk.img

ISO_DIR = iso

all: os.iso

build: kernel.elf build_apps

kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o kernel.elf

os.iso: kernel.elf create_bin_folder create_harddisk_image

	mkdir -p $(ISO_DIR)/boot/grub
	cp kernel.elf $(ISO_DIR)/boot/kernel.elf
	cp grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o os.iso $(ISO_DIR)

create_harddisk_image:
	@echo '-> Creating harddisk image';
	@make --no-print-directory -C $(HDDIR);
	@echo ''; \

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
	qemu-system-i386 -boot d -cdrom os.iso -hdb $(HD) -m 256M -serial file:com1.qemu.out
qemugdb: os.iso
	#qemu-system-i386 -s -S -m 32M -boot c -hda master.img
	qemu-system-i386 -s -S -m 256M -boot d -cdrom os.iso -hdb $(HD)

%.o: %.c
	$(CC) $(CFLAGS)  $< -o $@

%.o: %.s
	./c_to_nasm.sh $(AS_HEADERS)
	$(AS) $(ASFLAGS) $< -o $@

clean:
	@echo '-> Cleaning apps'
	@make --no-print-directory -C apps clean
	@echo ''
	@echo '-> Cleaning harddisk'
	@make --no-print-directory -C $(HDDIR) clean
	@echo ''
	@echo '-> Cleaning bin'
	if [ -d $(BIN_PATH) ]; then rm $(BIN_PATH) -rf; fi
	rm -rf *.o kernel.elf os.iso *.out $(ISO_DIR) *.img
	find . -name '*.o' -type f -delete

clang-format:
	clang-format -i *.c *.h */*.c */*.h

