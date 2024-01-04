/** @file */

#include "check.h"
#include "constants.h"
#include "framebuffer.h"
#include "interrupts.h"
#include "keyboard.h"
#include "paging.h"
#include "segments.h"
#include "serial.h"
#include "stdio.h"

char welcome[] = "Little OS";

void kmain() {
    fb_clear();
    printf("Hello %s\n", welcome);
    multiboot_check();
    kernel_check();
    serial_write(welcome, sizeof(welcome));
    segments_install_gdt();
    keyboard_init();
    interrupts_install_idt();
    initialise_paging();
}
