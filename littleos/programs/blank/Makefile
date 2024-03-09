OBJECTS = start.o

AS = nasm
CC = i686-elf-gcc
LD = $(CC)

ASFLAGS = -f elf
CFLAGS = -std=gnu99 -O2 -Wall -Wextra -Werror -c
LDFLAGS = -T link.ld -O2 -nostdlib

BIN_FOLDER = ../../bin

all: blank.bin

blank.bin: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o blank.bin

copy_binaries:
	cp blank.bin $(BIN_FOLDER)

%.o: %.c
	$(CC) $(CFLAGS) $(STDINCLUDE) $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf *.bin *.o
