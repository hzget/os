#ifndef PROCESS_H
#define PROCESS_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "config.h"
#include "task.h"

#define PROCESS_FILETYPE_ELF 0
#define PROCESS_FILETYPE_BINARY 1

typedef unsigned char PROCESS_FILETYPE;

struct process_allocation {
    void *ptr;
    size_t size;
};

struct command_argument {
    char argument[512];
    struct command_argument *next;
};

struct process_arguments {
    int argc;
    char **argv;
};

struct process {
    uint16_t id;

    char filename[MAX_FILENAME_PATH];

    // The main process task
    struct task *task;

    // The memory (malloc) allocations of the process
    struct process_allocation allocations[MAX_PROGRAM_ALLOCATIONS];

    PROCESS_FILETYPE filetype;

    // The physical pointer to the process memory.
    void *ptr;

    // The physical pointer to the stack memory
    void *stack;

    // The size of the data pointed to by "ptr"
    uint32_t size;

    // The arguments of the process.
    struct process_arguments arguments;
};

int process_switch(struct process *process);
int process_load_switch(const char *filename, struct process **process);
int process_load(const char *filename, struct process **process);
int process_load_for_slot(const char *filename, struct process **process,
                          int process_slot);
struct process *process_current();
struct process *process_get(int process_id);
void *process_malloc(struct process *process, size_t size);
void process_free(struct process *process, void *ptr);

void process_get_arguments(struct process *process, int *argc, char ***argv);
int process_inject_arguments(struct process *process,
                             struct command_argument *root_argument);
int process_terminate(struct process *process);

#endif
