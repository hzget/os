/** @file */
/*
 * FAT16 File System Structure
 *
 *   Reserved Region (incl. Boot Sector)
 *   File Allocation Table (FAT)
 *   Root Directory
 *   Data Region
 *
 * Please refer to following links for details:
 *
 *   http://www.maverick-os.dk/FileSystemFormats/FAT16_FileSystem.html
 *   https://wiki.osdev.org/FAT
 *
 */

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

/* Structure of the FAT16 Boot sector
 *
 *   Code - Code to jump to the bootstrap code
 *   OS Name
 *   BIOS Parameter Block
 *   Extended BIOS Parameter Block
 *   Bootstrap Code
 *   Boot Sector Signature - 0xAA55
 */
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

// Attention: signature field must be FAT16_SIGNATURE (0x29)
struct fat_header_extended {
    uint8_t drive_number;
    uint8_t win_nt_bit;
    uint8_t signature;
    uint32_t volume_id;
    uint8_t volume_id_string[11];
    uint8_t system_id_string[8];
} __attribute__((packed));

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

// following is internal structures
typedef struct fat_h fat_h_t;
typedef struct fat_directory fat_directory_t;
typedef struct fat_item fat_item_t;
typedef struct fat_private fat_private_t;

struct fat_h {
    struct fat_header primary_header;
    union fat_h_e {
        struct fat_header_extended extended_header;
    } shared;
};

struct fat_directory {
    struct fat_directory_item *item;
    int total;
    int start_sector;
    int end_sector;
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
    struct fat_directory root_dir;

    // Used to stream data clusters
    disk_stream_t *cluster_stream;
    // Used to stream the file allocation table
    disk_stream_t *fat_stream;
    // Used in situations where we stream the directory
    disk_stream_t *directory_stream;
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

static void fat16_init_private(disk_t *disk, fat_private_t *private) {
    memset((uint8_t *)private, 0, sizeof(fat_private_t));
    private->cluster_stream = diskstreamer_new(disk->id);
    private->fat_stream = diskstreamer_new(disk->id);
    private->directory_stream = diskstreamer_new(disk->id);
}

int fat16_get_total_items_for_directory(disk_t *disk, uint32_t start_sector) {
    int res = 0;
    int i = 0;
    int pos = 0;
    fat_private_t *private;

    struct fat_directory_item item;
    memset((uint8_t *)&item, 0, sizeof(item));
    pos = start_sector * disk->sector_size;
    private = disk->fs_private;
    disk_stream_t *stream = private->directory_stream;
    res = diskstreamer_seek(stream, pos);
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

int fat16_get_root_dir(disk_t *disk, fat_private_t *private,
                       fat_directory_t *directory) {
    int res = 0;
    int start_sector = 0;
    int total_sectors = 0;
    int entries = 0;
    int size = 0;
    int total_items = 0;

    struct fat_directory_item *dir = 0x00;
    struct fat_header *h = &private->header.primary_header;

    start_sector = h->reserved_sectors + (h->fat_copies * h->sectors_per_fat);
    entries = private->header.primary_header.root_dir_entries;
    size = (entries * sizeof(struct fat_directory_item));
    total_sectors = size / disk->sector_size;
    if (size % disk->sector_size) {
        total_sectors += 1;
    }

    total_items = fat16_get_total_items_for_directory(disk, start_sector);

    dir = kcalloc(size);
    if (!dir) {
        res = -ENOMEM;
        goto err;
    }

    disk_stream_t *stream = private->directory_stream;
    res = diskstreamer_seek(stream, start_sector * disk->sector_size);
    if (!SUCCESS(res)) {
        goto err;
    }

    res = diskstreamer_read(stream, dir, size);
    if (!SUCCESS(res)) {
        goto err;
    }

    directory->item = dir;
    directory->total = total_items;
    directory->start_sector = start_sector;
    directory->end_sector = start_sector + total_sectors;

    goto end;

err:
    if (dir) {
        kfree(dir);
    }

end:
    return res;
}

int fat16_resolve(disk_t *disk) {
    int res = 0;
    fat_private_t *private = (fat_private_t *)kcalloc(sizeof(fat_private_t));
    fat16_init_private(disk, private);

    disk->fs_private = private;
    disk->filesystem = &fat16_fs;

    disk_stream_t *stream = diskstreamer_new(disk->id);
    if (!stream) {
        res = -ENOMEM;
        goto out;
    }

    res = diskstreamer_read(stream, &private->header, sizeof(private->header));
    if (!SUCCESS(res)) {
        goto out;
    }

    if (private->header.shared.extended_header.signature != FAT16_SIGNATURE) {
        res = -EFSNOTUS;
        goto out;
    }

    res = fat16_get_root_dir(disk, private, &private->root_dir);

out:
    if (stream) {
        diskstreamer_close(stream);
    }

    if (res < 0) {
        kfree(private);
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
