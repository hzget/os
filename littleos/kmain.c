
#include "framebuffer.h"
#include "serial.h"

char welcome[] = "Little OS";

void kmain() {
    fb_write(welcome, sizeof(welcome));
    serial_write(welcome, sizeof(welcome));
}

