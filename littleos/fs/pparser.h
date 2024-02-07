#ifndef PATHPARSER_H
#define PATHPARSER_H

#define FS_PATH_MAX_LEN 108

typedef struct path_root path_root_t;
typedef struct path_part path_part_t;

struct path_root {
    int drive_no;
    struct path_part *first;
};

struct path_part {
    const char *part;
    struct path_part *next;
};

path_root_t *pathparser_parse(const char *path);
void pathparser_free(path_root_t *root);

#endif
