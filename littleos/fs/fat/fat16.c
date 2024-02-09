#include "fat16.h"
#include "file.h"
#include "status.h"
#include "string.h"
#include <stdint.h>

static int fat16_resolve(disk_t *disk);
static void *fat16_open(disk_t *disk, path_part_t *path, FILE_MODE mode);

filesystem_t fat16_fs = {
    .resolve = fat16_resolve,
    .open = fat16_open,
};

filesystem_t *fat16_init() {
    strcpy(fat16_fs.name, "FAT16");
    return &fat16_fs;
}

static int fat16_resolve(disk_t *disk) {
    (void)disk;
    return STATUS_OK;
    // return -ENOMEM;
}

static void *fat16_open(disk_t *disk, path_part_t *path, FILE_MODE mode) {
    (void)disk;
    (void)path;
    (void)mode;
    return 0;
}
