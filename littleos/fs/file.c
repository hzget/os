#include "file.h"
#include "config.h"
#include "fat16.h"
#include "kheap.h"
#include "status.h"
#include "stdio.h"

struct filesystem *filesystems[MAX_FILESYSTEMS];
struct file_descriptor *file_descriptors[MAX_FILE_DESCRIPTORS];

static struct filesystem **fs_get_free_filesystem() {
    int i;
    for (i = 0; i < MAX_FILESYSTEMS; i++) {
        if (filesystems[i] == 0) {
            return &filesystems[i];
        }
    }

    return 0;
}

void fs_insert_filesystem(struct filesystem *filesystem) {
    struct filesystem **fs;
    fs = fs_get_free_filesystem();
    if (!fs) {
        PANIC("Problem inserting filesystem");
    }

    *fs = filesystem;
}

static void fs_static_load() {
    fs_insert_filesystem(fat16_init());
}

void fs_load() {
    fs_static_load();
}

void init_fs() {
    fs_load();
}

struct filesystem *fs_resolve(struct disk *disk) {
    struct filesystem *fs = 0;
    for (int i = 0; i < MAX_FILESYSTEMS; i++) {
        if (filesystems[i] != 0 && filesystems[i]->resolve(disk) == 0) {
            fs = filesystems[i];
            break;
        }
    }

    return fs;
}
