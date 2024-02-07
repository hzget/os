#include "pparser.h"
#include "kheap.h"
#include "status.h"
#include "string.h"

static int pathparser_path_valid_format(const char *filename) {
    int len = strnlen(filename, FS_PATH_MAX_LEN);
    return (len >= 3 && isdigit(filename[0]) &&
            memcmp((void *)&filename[1], (uint8_t *)":/", 2) == 0);
}

static int pathparser_get_drive_by_path(const char **path) {
    if (!pathparser_path_valid_format(*path)) {
        return -EBADPATH;
    }

    int drive_no = tonumericdigit(*path[0]);

    // Add 3 bytes to skip drive number 0:/ 1:/ 2:/
    *path += 3;
    return drive_no;
}

static path_root_t *pathparser_create_root(int drive_number) {
    path_root_t *path_r = kcalloc(sizeof(path_root_t));
    path_r->drive_no = drive_number;
    path_r->first = 0;
    return path_r;
}

static const char *pathparser_get_path_part(const char **path) {
    char *result_path_part = kcalloc(FS_PATH_MAX_LEN);
    int i = 0;
    while (**path != '/' && **path != 0x00) {
        result_path_part[i] = **path;
        *path += 1;
        i++;
    }

    if (**path == '/') {
        // Skip the forward slash to avoid problems
        *path += 1;
    }

    if (i == 0) {
        kfree(result_path_part);
        result_path_part = 0;
    }

    return result_path_part;
}

path_part_t *pathparser_parse_path_part(path_part_t *last_part,
                                        const char **path) {
    const char *path_part_str = pathparser_get_path_part(path);
    if (!path_part_str) {
        return 0;
    }

    path_part_t *part = kcalloc(sizeof(path_part_t));
    part->part = path_part_str;
    part->next = 0x00;

    if (last_part) {
        last_part->next = part;
    }

    return part;
}

void pathparser_free(path_root_t *root) {
    path_part_t *part = root->first;
    while (part) {
        path_part_t *next_part = part->next;
        kfree((void *)part->part);
        kfree(part);
        part = next_part;
    }

    kfree(root);
}

path_root_t *pathparser_parse(const char *path) {
    int res = 0;
    const char *tmp_path = path;
    path_root_t *path_root = 0;

    if (strlen(path) > FS_PATH_MAX_LEN) {
        goto out;
    }

    res = pathparser_get_drive_by_path(&tmp_path);
    if (res < 0) {
        goto out;
    }

    path_root = pathparser_create_root(res);
    if (!path_root) {
        goto out;
    }

    path_part_t *first_part = pathparser_parse_path_part(NULL, &tmp_path);
    if (!first_part) {
        goto out;
    }

    path_root->first = first_part;
    path_part_t *part = pathparser_parse_path_part(first_part, &tmp_path);
    while (part) {
        part = pathparser_parse_path_part(part, &tmp_path);
    }

out:
    return path_root;
}
