OBJECTS = hello.o start.o

AS = nasm
#AS = i686-elf-as
CC = i686-elf-gcc
LD = $(CC)

ASFLAGS = -f elf
CFLAGS = -std=gnu99 -O2 -Wall -Wextra -Werror -c
LDFLAGS = -T link.ld -O2 -nostdlib



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
