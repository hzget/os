OBJECTS = hello.o start.o
CC = gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
		 -fno-pie -g \
		 -nostartfiles -nodefaultlibs -Wall -Wextra -Werror \
		 -Wno-unused-function -c
AS = nasm
ASFLAGS = -f elf
LD = ld
LDFLAGS = -T link.ld -melf_i386
LIBC = -L../libc -lc
STDINCLUDE = -I../libc

all: main

main: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o main

%.o: %.c
	$(CC) $(CFLAGS) $(STDINCLUDE) $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf main *.o
