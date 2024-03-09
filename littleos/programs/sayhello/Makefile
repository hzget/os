OBJECTS = start.o

target = sayhello.bin

AS = nasm
CC = i686-elf-gcc
LD = $(CC)

ASFLAGS = -f elf
CFLAGS = -std=gnu99 -O2 -Wall -Wextra -Werror -c
LDFLAGS = -T link.ld -O2 -nostdlib

BIN_FOLDER = ../../bin

all: $(target)

$(target): $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o $(target)

copy_binaries:
	cp $(target) $(BIN_FOLDER)

%.o: %.c
	$(CC) $(CFLAGS) $(STDINCLUDE) $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf *.bin *.o
