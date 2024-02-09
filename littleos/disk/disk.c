/** @file */
#include "disk.h"
#include "io.h"
#include "status.h"
#include <stddef.h>

disk_t disk;

/** disk_read_sector
 *  reads data from harddisk to a buffer.
 *  It uses the slave drive in the primary bus by default. (drive #1)
 *  ATA protocol is used to transfer the data.
 *
 *  @param lba    the start sector no
 *  @param total  num of sectors to read
 *  @param buf    the buffer to store the data
 *  @return       0 if success
 *                #err if failed
 */
static int32_t disk_read_sector(uint32_t lba, int32_t total, void *buf) {
    outb(0x1F6, (lba >> 24) | 0xF0);
    outb(0x1F2, total);
    outb(0x1F3, (uint8_t)(lba & 0xff));
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));
    outb(0x1F7, 0x20);

    uint16_t *ptr = (uint16_t *)buf;
    for (int32_t b = 0; b < total; b++) {
        // Wait ready status
        uint8_t c = inb(0x1F7);
        while (!(c & 0x08)) {
            c = inb(0x1F7);
        }

        for (int16_t i = 0; i < 256; i++) {
            *ptr = inw(0x1F0);
            ptr++;
        }
    }
    return 0;
}

void init_disk() {
    disk.type = OS_DISK_TYPE_REAL;
    disk.sector_size = OS_SECTOR_SIZE;
    disk.filesystem = fs_resolve(&disk);
}

disk_t *disk_get(int8_t drive) {
    if (drive != HD_DRIVE_SLAVE) {
        return NULL;
    }

    return &disk;
}

/** disk_read_block
 *  reads data from harddisk to a buffer.
 *  ATA protocol is used to transfer the data.
 *
 *  @param idisk
 *  @param lba    the start sector no
 *  @param total  num of sectors to read
 *  @param buf    the buffer to store the data
 *  @return       0 if success
 *                #err if failed
 */
int32_t disk_read_block(disk_t *idisk, uint32_t lba, int32_t total, void *buf) {
    if (idisk != &disk) {
        return -EIO;
    }

    return disk_read_sector(lba, total, buf);
}
