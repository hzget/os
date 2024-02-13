/** @file */
#ifndef DISK_H
#define DISK_H

#include "file.h"
#include <stdint.h>

#define OS_DISK_TYPE_REAL 0

#define OS_SECTOR_SIZE 512

enum {
    HD_DRIVE_MASTER = 0,
    HD_DRIVE_SLAVE = 1,
};

typedef struct disk disk_t;

struct disk {
    uint8_t type;
    uint32_t sector_size;
    struct filesystem *filesystem;

    uint8_t id;

    // The private data of our filesystem
    void *fs_private;
};

void init_disk();
disk_t *disk_get(int8_t drive);
int32_t disk_read_block(disk_t *idisk, uint32_t lba, int32_t total, void *buf);

#endif
