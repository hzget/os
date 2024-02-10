#include "fat16.h"
#include "check.h"
#include "file.h"
#include "kheap.h"
#include "status.h"
#include "streamer.h"
#include "string.h"
#include <stdint.h>

#define FAT16_SIGNATURE 0x29
#define FAT16_FAT_ENTRY_SIZE 0x02
#define FAT16_BAD_SECTOR 0xFF7
#define FAT16_UNUSED 0x00

typedef uint32_t FAT_ITEM_TYPE;
#define FAT_ITEM_TYPE_DIRECTORY 0
#define FAT_ITEM_TYPE_FILE 1

// Fat directory entry attributes bitmask
#define FAT_FILE_READ_ONLY 0x01
#define FAT_FILE_HIDDEN 0x02
#define FAT_FILE_SYSTEM 0x04
#define FAT_FILE_VOLUME_LABEL 0x08
#define FAT_FILE_SUBDIRECTORY 0x10
#define FAT_FILE_ARCHIVED 0x20
#define FAT_FILE_DEVICE 0x40
#define FAT_FILE_RESERVED 0x80

typedef struct fat_private fat_private_t;

struct fat_header_extended {
    uint8_t drive_number;
    uint8_t win_nt_bit;
    uint8_t signature;
    uint32_t volume_id;
    uint8_t volume_id_string[11];
    uint8_t system_id_string[8];
} __attribute__((packed));

struct fat_header {
    uint8_t short_jmp_ins[3];
    uint8_t oem_identifier[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_copies;
    uint16_t root_dir_entries;
    uint16_t number_of_sectors;
    uint8_t media_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t number_of_heads;
    uint32_t hidden_setors;
    uint32_t sectors_big;
} __attribute__((packed));

struct fat_h {
    struct fat_header primary_header;
    union fat_h_e {
        struct fat_header_extended extended_header;
    } shared;
};

struct fat_directory_item {
    uint8_t filename[8];
    uint8_t ext[3];
    uint8_t attribute;
    uint8_t reserved;
    uint8_t creation_time_tenths_of_a_sec;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access;
    uint16_t high_16_bits_first_cluster;
    uint16_t last_mod_time;
    uint16_t last_mod_date;
    uint16_t low_16_bits_first_cluster;
    uint32_t filesize;
} __attribute__((packed));

struct fat_directory {
    struct fat_directory_item *item;
    int total;
    int sector_pos;
    int ending_sector_pos;
};

struct fat_item {
    union {
        struct fat_directory_item *item;
        struct fat_directory *directory;
    };

    FAT_ITEM_TYPE type;
};

struct fat_file_descriptor {
    struct fat_item *item;
    uint32_t pos;
};

struct fat_private {
    struct fat_h header;
    struct fat_directory root_directory;

    // Used to stream data clusters
    struct disk_stream *cluster_read_stream;
    // Used to stream the file allocation table
    struct disk_stream *fat_read_stream;

    // Used in situations where we stream the directory
    struct disk_stream *directory_stream;
};

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

static void fat16_init_private(struct disk *disk, fat_private_t *private) {
    memset((uint8_t *)private, 0, sizeof(fat_private_t));
    private->cluster_read_stream = diskstreamer_new(disk->id);
    private->fat_read_stream = diskstreamer_new(disk->id);
    private->directory_stream = diskstreamer_new(disk->id);
}

int fat16_sector_to_absolute(struct disk *disk, int sector) {
    return sector * disk->sector_size;
}

int fat16_get_total_items_for_directory(struct disk *disk,
                                        uint32_t directory_start_sector) {
    struct fat_directory_item item;
    struct fat_directory_item empty_item;
    memset((uint8_t *)&empty_item, 0, sizeof(empty_item));

    fat_private_t *fat_private = disk->fs_private;

    int res = 0;
    int i = 0;
    int directory_start_pos = directory_start_sector * disk->sector_size;
    struct disk_stream *stream = fat_private->directory_stream;
    res = diskstreamer_seek(stream, directory_start_pos);
    if (!SUCCESS(res)) {
        goto out;
    }

    while (1) {
        res = diskstreamer_read(stream, &item, sizeof(item));
        if (!SUCCESS(res)) {
            goto out;
        }

        if (item.filename[0] == 0x00) {
            // We are done
            break;
        }

        // Is the item unused
        if (item.filename[0] == 0xE5) {
            continue;
        }

        i++;
    }

    res = i;

out:
    return res;
}

int fat16_get_root_directory(struct disk *disk, fat_private_t *fat_private,
                             struct fat_directory *directory) {
    int res = 0;
    struct fat_directory_item *dir = 0x00;
    struct fat_header *primary_header = &fat_private->header.primary_header;
    int root_dir_sector_pos =
        (primary_header->fat_copies * primary_header->sectors_per_fat) +
        primary_header->reserved_sectors;
    int root_dir_entries = fat_private->header.primary_header.root_dir_entries;
    int root_dir_size = (root_dir_entries * sizeof(struct fat_directory_item));
    int total_sectors = root_dir_size / disk->sector_size;
    if (root_dir_size % disk->sector_size) {
        total_sectors += 1;
    }

    int total_items =
        fat16_get_total_items_for_directory(disk, root_dir_sector_pos);

    dir = kcalloc(root_dir_size);
    if (!dir) {
        res = -ENOMEM;
        goto err;
    }

    struct disk_stream *stream = fat_private->directory_stream;
    res = diskstreamer_seek(
        stream, fat16_sector_to_absolute(disk, root_dir_sector_pos));
    if (!SUCCESS(res)) {
        goto err;
    }

    res = diskstreamer_read(stream, dir, root_dir_size);
    if (!SUCCESS(res)) {
        goto err;
    }

    directory->item = dir;
    directory->total = total_items;
    directory->sector_pos = root_dir_sector_pos;
    directory->ending_sector_pos =
        root_dir_sector_pos + (root_dir_size / disk->sector_size);

    goto out;

err:
    if (dir) {
        kfree(dir);
    }

out:
    return res;
}

int fat16_resolve(struct disk *disk) {
    int res = 0;
    fat_private_t *fat_private =
        (fat_private_t *)kcalloc(sizeof(fat_private_t));
    fat16_init_private(disk, fat_private);

    disk->fs_private = fat_private;
    disk->filesystem = &fat16_fs;

    struct disk_stream *stream = diskstreamer_new(disk->id);
    if (!stream) {
        res = -ENOMEM;
        goto out;
    }

    res = diskstreamer_read(stream, &fat_private->header,
                            sizeof(fat_private->header));
    if (!SUCCESS(res)) {
        goto out;
    }

    if (fat_private->header.shared.extended_header.signature != 0x29) {
        res = -EFSNOTUS;
        goto out;
    }

    res = fat16_get_root_directory(disk, fat_private,
                                   &fat_private->root_directory);
    if (!SUCCESS(res)) {
        goto out;
    }

out:
    if (stream) {
        diskstreamer_close(stream);
    }

    if (res < 0) {
        kfree(fat_private);
        disk->fs_private = 0;
    }
    return res;
}

static void *fat16_open(disk_t *disk, path_part_t *path, FILE_MODE mode) {
    (void)disk;
    (void)path;
    (void)mode;
    return 0;
}
