
#include "framebuffer.h"

char welcome[] = "Little OS";

void kmain() {
    fb_move_cursor(80*7);
    fb_write_str(welcome, sizeof(welcome));
}

