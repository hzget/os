/** @file */

#include "framebuffer.h"
#include "segments.h"
#include "serial.h"

char welcome[] = "Little OS";

void kmain() {
    fb_write(welcome, sizeof(welcome));
    serial_write(welcome, sizeof(welcome));
    segments_install_gdt();
}
