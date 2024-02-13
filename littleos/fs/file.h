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

enum {
    FILE_STAT_READ_ONLY = 1 << 0,
};

typedef unsigned int FILE_STAT_FLAGS;

struct disk;
typedef void *(*fs_open_func)(struct disk *disk, path_part_t *path,
                              FILE_MODE mode, int32_t *err_code);
typedef int (*fs_resolve_func)(struct disk *disk);
typedef int (*fs_read_func)(struct disk *disk, void *private, uint32_t size,
                            uint32_t nmemb, char *out);
typedef int (*fs_close_func)(void *private);

typedef int (*fs_seek_func)(void *private, uint32_t offset,
                            FILE_SEEK_MODE seek_mode);

struct file_stat {
    FILE_STAT_FLAGS flags;
    uint32_t filesize;
};

typedef int (*fs_stat_func)(struct disk *disk, void *private,
                            struct file_stat *stat);

typedef struct filesystem filesystem_t;

struct filesystem {
    // Filesystem should return zero from resolve if the provided disk is using
    // its filesystem
    fs_resolve_func resolve;
    fs_open_func open;
    fs_read_func read;
    fs_seek_func seek;
    fs_stat_func stat;
    fs_close_func close;
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
int fseek(int fd, int offset, FILE_SEEK_MODE whence);
int fstat(int fd, struct file_stat *stat);
int fclose(int fd);

#endif
