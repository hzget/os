#ifndef DISKSTREAMER_H
#define DISKSTREAMER_H

#include "disk.h"

typedef struct disk_stream disk_stream_t;

struct disk_stream {
    int32_t pos;
    struct disk *disk;
};

disk_stream_t *diskstreamer_new(int8_t disk_id);
void diskstreamer_seek(disk_stream_t *stream, int32_t pos);
int32_t diskstreamer_read(disk_stream_t *stream, void *out, int32_t total);
void diskstreamer_close(disk_stream_t *stream);

#endif
