#include "disk.h"
#include "io.h"

int disk_read_sector(int lba, int total, void *buf) {
    // use slave drive
    outb(0x1F6, (lba >> 24) | 0xF0);
    outb(0x1F2, total);
    outb(0x1F3, (uint8_t)(lba & 0xff));
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));
    outb(0x1F7, 0x20);

    uint16_t *ptr = (uint16_t *)buf;
    for (int b = 0; b < total; b++) {
        // Wait ready status
        uint8_t c = inb(0x1F7);
        while (!(c & 0x08)) {
            c = inb(0x1F7);
        }

        for (int i = 0; i < 256; i++) {
            *ptr = inw(0x1F0);
            ptr++;
        }
    }
    return 0;
}
