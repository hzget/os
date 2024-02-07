/** @file */
#ifndef DISK_H
#define DISK_H

#include <stdint.h>

typedef uint8_t OS_DISK_TYPE;
#define OS_DISK_TYPE_REAL 0

#define OS_SECTOR_SIZE 512

enum {
    HD_DRIVE_MASTER = 0,
    HD_DRIVE_SLAVE = 1,
};

typedef struct disk disk_t;

struct disk {
    OS_DISK_TYPE type;
    uint32_t sector_size;
};

void init_disk();
disk_t *disk_get(int8_t drive);
int32_t disk_read_block(disk_t *idisk, uint32_t lba, uint8_t total, void *buf);

#endif
