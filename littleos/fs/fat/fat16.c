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
#include "config.h"
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
    } shared;

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
static void *fat16_open(disk_t *disk, path_part_t *path, FILE_MODE mode,
                        int32_t *ecode);
static int fat16_read(struct disk *disk, void *descriptor, uint32_t size,
                      uint32_t nmemb, char *out_ptr);

filesystem_t fat16_fs = {
    .resolve = fat16_resolve,
    .open = fat16_open,
    .read = fat16_read,
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

void fat16_to_proper_string(char **out, const char *in, size_t size) {
    size_t i = 0;
    while (*in != 0x00 && *in != 0x20) {
        **out = *in;
        *out += 1;
        in += 1;
        // We cant process anymore since we have exceeded the input buffer size
        if (i >= size - 1) {
            break;
        }
        i++;
    }

    **out = 0x00;
}

void fat16_get_full_relative_filename(struct fat_directory_item *item,
                                      char *out, int max_len) {
    memset((uint8_t *)out, 0x00, max_len);
    char *out_tmp = out;
    fat16_to_proper_string(&out_tmp, (const char *)item->filename,
                           sizeof(item->filename));
    if (item->ext[0] != 0x00 && item->ext[0] != 0x20) {
        *out_tmp++ = '.';
        fat16_to_proper_string(&out_tmp, (const char *)item->ext,
                               sizeof(item->ext));
    }
}

struct fat_directory_item *
fat16_clone_directory_item(struct fat_directory_item *item, size_t size) {
    struct fat_directory_item *item_copy = 0;
    if (size < sizeof(struct fat_directory_item)) {
        return 0;
    }

    item_copy = kcalloc(size);
    if (!item_copy) {
        return 0;
    }

    memcpy((uint8_t *)item_copy, (uint8_t *)item, size);
    return item_copy;
}

static uint32_t fat16_get_first_cluster(struct fat_directory_item *item) {
    return (item->high_16_bits_first_cluster) | item->low_16_bits_first_cluster;
}

static int fat16_cluster_to_sector(struct fat_private *private, int cluster) {
    return private->root_dir.end_sector +
           ((cluster - 2) * private->header.primary_header.sectors_per_cluster);
}

static uint32_t fat16_get_first_fat_sector(struct fat_private *private) {
    return private->header.primary_header.reserved_sectors;
}

static int fat16_get_fat_entry(disk_t *disk, int cluster) {
    int res = -1;
    struct fat_private *private = disk->fs_private;
    disk_stream_t *stream = private->fat_stream;
    if (!stream) {
        goto out;
    }

    uint32_t fat_table_position =
        fat16_get_first_fat_sector(private) * disk->sector_size;
    res = diskstreamer_seek(stream, fat_table_position *
                                        (cluster * FAT16_FAT_ENTRY_SIZE));
    if (res < 0) {
        goto out;
    }

    uint16_t result = 0;
    res = diskstreamer_read(stream, &result, sizeof(result));
    if (res < 0) {
        goto out;
    }

    res = result;
out:
    return res;
}
/**
 * Gets the correct cluster to use based on the starting cluster and the offset
 */
static int fat16_get_cluster_for_offset(disk_t *disk, int starting_cluster,
                                        int offset) {
    int res = 0;
    struct fat_private *private = disk->fs_private;
    int size_of_cluster_bytes =
        private->header.primary_header.sectors_per_cluster * disk->sector_size;
    int cluster_to_use = starting_cluster;
    int clusters_ahead = offset / size_of_cluster_bytes;
    for (int i = 0; i < clusters_ahead; i++) {
        int entry = fat16_get_fat_entry(disk, cluster_to_use);
        if (entry == 0xFF8 || entry == 0xFFF) {
            // We are at the last entry in the file
            res = -EIO;
            goto out;
        }

        // Sector is marked as bad?
        if (entry == FAT16_BAD_SECTOR) {
            res = -EIO;
            goto out;
        }

        // Reserved sector?
        if (entry == 0xFF0 || entry == 0xFF6) {
            res = -EIO;
            goto out;
        }

        if (entry == 0x00) {
            res = -EIO;
            goto out;
        }

        cluster_to_use = entry;
    }

    res = cluster_to_use;
out:
    return res;
}

static int fat16_read_internal_from_stream(disk_t *disk, disk_stream_t *stream,
                                           int cluster, int offset, int total,
                                           void *out) {
    int res = 0;
    struct fat_private *private = disk->fs_private;
    int size_of_cluster_bytes =
        private->header.primary_header.sectors_per_cluster * disk->sector_size;
    int cluster_to_use = fat16_get_cluster_for_offset(disk, cluster, offset);
    if (cluster_to_use < 0) {
        res = cluster_to_use;
        goto out;
    }

    int offset_from_cluster = offset % size_of_cluster_bytes;

    int starting_sector = fat16_cluster_to_sector(private, cluster_to_use);
    int starting_pos =
        (starting_sector * disk->sector_size) + offset_from_cluster;
    int total_to_read =
        total > size_of_cluster_bytes ? size_of_cluster_bytes : total;
    res = diskstreamer_seek(stream, starting_pos);
    if (!SUCCESS(res)) {
        goto out;
    }

    res = diskstreamer_read(stream, out, total_to_read);
    if (!SUCCESS(res)) {
        goto out;
    }

    total -= total_to_read;
    if (total > 0) {
        // We still have more to read
        // cluster = cluster_to_use // ??
        res = fat16_read_internal_from_stream(disk, stream, cluster,
                                              offset + total_to_read, total,
                                              (uint8_t *)out + total_to_read);
    }

out:
    return res;
}

static int fat16_read_internal(disk_t *disk, int starting_cluster, int offset,
                               int total, void *out) {
    struct fat_private *fs_private = disk->fs_private;
    disk_stream_t *stream = fs_private->cluster_stream;
    return fat16_read_internal_from_stream(disk, stream, starting_cluster,
                                           offset, total, out);
}

void fat16_free_directory(struct fat_directory *directory) {
    if (!directory) {
        return;
    }

    if (directory->item) {
        kfree(directory->item);
    }

    kfree(directory);
}

void fat16_fat_item_free(struct fat_item *item) {
    if (item->type == FAT_ITEM_TYPE_DIRECTORY) {
        fat16_free_directory(item->shared.directory);
    } else if (item->type == FAT_ITEM_TYPE_FILE) {
        kfree(item->shared.item);
    }

    kfree(item);
}

struct fat_directory *
fat16_load_fat_directory(disk_t *disk, struct fat_directory_item *item) {
    int res = 0;
    struct fat_directory *directory = 0;
    struct fat_private *fat_private = disk->fs_private;
    if (!(item->attribute & FAT_FILE_SUBDIRECTORY)) {
        res = -EINVARG;
        goto out;
    }

    directory = kcalloc(sizeof(struct fat_directory));
    if (!directory) {
        res = -ENOMEM;
        goto out;
    }

    int cluster = fat16_get_first_cluster(item);
    int cluster_sector = fat16_cluster_to_sector(fat_private, cluster);
    int total_items = fat16_get_total_items_for_directory(disk, cluster_sector);
    directory->total = total_items;
    int directory_size = directory->total * sizeof(struct fat_directory_item);
    directory->item = kcalloc(directory_size);
    if (!directory->item) {
        res = -ENOMEM;
        goto out;
    }

    res = fat16_read_internal(disk, cluster, 0x00, directory_size,
                              directory->item);
    if (!SUCCESS(res)) {
        goto out;
    }

out:
    if (!SUCCESS(res)) {
        fat16_free_directory(directory);
    }
    return directory;
}

struct fat_item *
fat16_new_fat_item_for_directory_item(disk_t *disk,
                                      struct fat_directory_item *item) {
    struct fat_item *f_item = kcalloc(sizeof(struct fat_item));
    if (!f_item) {
        return 0;
    }

    if (item->attribute & FAT_FILE_SUBDIRECTORY) {
        f_item->shared.directory = fat16_load_fat_directory(disk, item);
        f_item->type = FAT_ITEM_TYPE_DIRECTORY;
        return f_item;
    }

    f_item->type = FAT_ITEM_TYPE_FILE;
    f_item->shared.item =
        fat16_clone_directory_item(item, sizeof(struct fat_directory_item));
    return f_item;
}

struct fat_item *fat16_find_item_in_directory(disk_t *disk,
                                              struct fat_directory *directory,
                                              const char *name) {
    struct fat_item *f_item = 0;
    char tmp_filename[MAX_FILENAME_PATH];
    for (int i = 0; i < directory->total; i++) {
        fat16_get_full_relative_filename(&directory->item[i], tmp_filename,
                                         sizeof(tmp_filename));
        if (strncasecmp(tmp_filename, name, sizeof(tmp_filename)) == 0) {
            // Found it let's create a new fat_item
            f_item = fat16_new_fat_item_for_directory_item(disk,
                                                           &directory->item[i]);
            break;
        }
    }

    return f_item;
}

struct fat_item *fat16_get_directory_entry(disk_t *disk, path_part_t *path) {
    struct fat_private *fat_private = disk->fs_private;
    struct fat_item *current_item = 0;
    struct fat_item *root_item =
        fat16_find_item_in_directory(disk, &fat_private->root_dir, path->part);
    if (!root_item) {
        goto out;
    }

    path_part_t *next_part = path->next;
    current_item = root_item;
    while (next_part != 0) {
        if (current_item->type != FAT_ITEM_TYPE_DIRECTORY) {
            current_item = 0;
            break;
        }

        struct fat_item *tmp_item = fat16_find_item_in_directory(
            disk, current_item->shared.directory, next_part->part);
        fat16_fat_item_free(current_item);
        current_item = tmp_item;
        next_part = next_part->next;
    }
out:
    return current_item;
}

void *fat16_open(struct disk *disk, struct path_part *path, FILE_MODE mode,
                 int32_t *ecode) {
    struct fat_file_descriptor *descriptor = 0;
    if (mode != FILE_MODE_READ) {
        *ecode = -ERDONLY;
        goto err_out;
    }

    descriptor = kcalloc(sizeof(struct fat_file_descriptor));
    if (!descriptor) {
        *ecode = -ENOMEM;
        goto err_out;
    }

    descriptor->item = fat16_get_directory_entry(disk, path);
    if (!descriptor->item) {
        *ecode = -EIO;
        goto err_out;
    }

    descriptor->pos = 0;
    *ecode = STATUS_OK;
    return descriptor;

err_out:
    if (descriptor) {
        kfree(descriptor);
    }

    return NULL;
}

static int fat16_read(struct disk *disk, void *descriptor, uint32_t size,
                      uint32_t nmemb, char *out_ptr) {
    int res = 0;
    struct fat_file_descriptor *fat_desc = descriptor;
    struct fat_directory_item *item = fat_desc->item->shared.item;
    int offset = fat_desc->pos;
    for (uint32_t i = 0; i < nmemb; i++) {
        res = fat16_read_internal(disk, fat16_get_first_cluster(item), offset,
                                  size, out_ptr);
        if (ISERR(res)) {
            goto out;
        }

        out_ptr += size;
        offset += size;
    }

    res = nmemb;
out:
    return res;
}
