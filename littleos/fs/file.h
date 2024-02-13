#ifndef FILE_H
#define FILE_H

#include "disk.h"
#include "pparser.h"
#include <stddef.h>
#include <stdint.h>

typedef uint32_t FILE_SEEK_MODE;
enum {
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

typedef uint32_t FILE_MODE;
enum {
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID
};

struct disk;
typedef void *(*fs_open_func)(struct disk *disk, path_part_t *path,
                              FILE_MODE mode, int32_t *err_code);
typedef int (*fs_resolve_func)(struct disk *disk);
typedef int (*fs_read_func)(struct disk *disk, void *private, uint32_t size,
                            uint32_t nmemb, char *out);

typedef struct filesystem filesystem_t;

struct filesystem {
    // Filesystem should return zero from resolve if the provided disk is using
    // its filesystem
    fs_resolve_func resolve;
    fs_open_func open;
    fs_read_func read;
    char name[20];
};

struct file_descriptor {
    // The descriptor index
    int index;
    struct filesystem *filesystem;

    // Private data for internal file descriptor
    void *private;

    // The disk that the file descriptor should be used on
    struct disk *disk;
};

void init_fs();
struct filesystem *fs_resolve(struct disk *disk);
int fopen(const char *filename, const char *mode_str);
int fread(void *ptr, uint32_t size, uint32_t nmemb, int fd);
#endif
