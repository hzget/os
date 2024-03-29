# Makefile for JamesM's kernel tutorials.
# The C and C++ rules are already setup by default.
# The only one that needs changing is the assembler 
# rule, as we use nasm instead of GNU as.

SOURCES=boot.o main.o monitor.o common.o stdio.o gdt.o interrupt.o isr.o descriptor_tables.o

CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
	-nostartfiles -nodefaultlibs -Wall -Wextra -Werror -g -c

LDFLAGS = -T link.ld -melf_i386

AS = nasm

ASFLAGS=-f elf

all: $(SOURCES) link

clean:
	rm -rf *.o kernel

link:
	ld $(LDFLAGS) -o kernel $(SOURCES)

%.o: %.c
	$(CC) $(CFLAGS)  $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clang-format:
	clang-format -i *.c *.h
