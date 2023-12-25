/** @file */

#include "framebuffer.h"
#include "serial.h"
#include "segments.h"

char welcome[] = "Little OS";

void kmain() {
    fb_write(welcome, sizeof(welcome));
    serial_write(welcome, sizeof(welcome));
    segments_install_gdt();
}

