#include "streamer.h"
#include "disk.h"
#include "kheap.h"
#include <stddef.h>

disk_stream_t *diskstreamer_new(int8_t disk_id) {
    struct disk *disk = disk_get(disk_id);
    if (!disk) {
        return NULL;
    }

    disk_stream_t *streamer = kcalloc(sizeof(disk_stream_t));
    streamer->pos = 0;
    streamer->disk = disk;
    return streamer;
}

void diskstreamer_seek(disk_stream_t *stream, int32_t pos) {
    stream->pos = pos;
}

int32_t diskstreamer_read(disk_stream_t *stream, void *out, int32_t total) {
    int32_t sector = stream->pos / OS_SECTOR_SIZE;
    int32_t offset = stream->pos % OS_SECTOR_SIZE;
    int32_t total_to_read = total;
    int32_t overflow = (offset + total_to_read) >= OS_SECTOR_SIZE;
    char buf[OS_SECTOR_SIZE];

    if (overflow) {
        total_to_read -= (offset + total_to_read) - OS_SECTOR_SIZE;
    }

    int32_t res = disk_read_block(stream->disk, sector, 1, buf);
    if (res < 0) {
        goto out;
    }

    for (int32_t i = 0; i < total_to_read; i++) {
        *(char *)out++ = buf[offset + i];
    }

    stream->pos += total_to_read;
    if (overflow) {
        res = diskstreamer_read(stream, out, total - total_to_read);
    }
out:
    return res;
}

void diskstreamer_close(disk_stream_t *stream) {
    kfree(stream);
}
