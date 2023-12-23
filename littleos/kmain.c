
#include "framebuffer.h"

char welcome[] = "Little OS";

void kmain() {
    write(welcome, sizeof(welcome));
}

