#include "process.h"
#include "check.h"
#include "config.h"
#include "constants.h"
#include "file.h"
#include "kheap.h"
#include "paging.h"
#include "stdio.h"
#include "string.h"

// The current process that is running
struct process *current_process = 0;

static struct process *processes[MAX_PROCESSES] = {0};

static void process_init(struct process *process) {
    memset(process, 0, sizeof(struct process));
}

struct process *process_current() {
    return current_process;
}

struct process *process_get(int process_id) {
    if (process_id < 0 || process_id >= MAX_PROCESSES) {
        return NULL;
    }

    return processes[process_id];
}

int process_switch(struct process *process) {
    current_process = process;
    return 0;
}

int process_free_binary_data(struct process *process) {
    kfree(process->ptr);
    return 0;
}

int process_free_program_data(struct process *process) {
    int res = 0;
    switch (process->filetype) {
    case PROCESS_FILETYPE_BINARY:
        res = process_free_binary_data(process);
        break;

    default:
        res = -EINVARG;
    }
    return res;
}

static int process_load_binary(const char *filename, struct process *process) {
    void *program_data_ptr = 0x00;
    int res = 0;
    int fd = fopen(filename, "r");
    if (!fd) {
        res = -EIO;
        goto out;
    }

    struct file_stat stat;
    res = fstat(fd, &stat);
    if (!SUCCESS(res)) {
        goto out;
    }

    program_data_ptr = kcalloc(stat.filesize);
    if (!program_data_ptr) {
        res = -ENOMEM;
        goto out;
    }

    if (fread(program_data_ptr, stat.filesize, 1, fd) != 1) {
        res = -EIO;
        goto out;
    }

    process->filetype = PROCESS_FILETYPE_BINARY;
    process->ptr = program_data_ptr;
    process->size = stat.filesize;

out:
    if (res < 0) {
        if (program_data_ptr) {
            kfree(program_data_ptr);
        }
    }
    fclose(fd);
    return res;
}

static int process_load_data(const char *filename, struct process *process) {
    int res = 0;
    res = process_load_binary(filename, process);

    return res;
}

int process_map_binary(struct process *process) {
    int res = 0;
    void *paddr;
    res = kheap_get_paddr(process->ptr, &paddr);
    if (res < 0) {
        goto out;
    }
    paging_map_to(
        process->task->page_directory, (void *)USER_CODE_VADDR, paddr,
        paging_align_address((void *)((uint32_t)paddr + process->size)),
        PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);
out:
    return res;
}

int process_map_memory(struct process *process) {
    int res = 0;

    // map code segment
    switch (process->filetype) {
    case PROCESS_FILETYPE_BINARY:
        res = process_map_binary(process);
        break;

    default:
        PANIC("process_map_memory: Invalid filetype\n");
    }

    if (res < 0) {
        goto out;
    }

    // map stack segment
    void *paddr;
    res = kheap_get_paddr(process->stack, &paddr);
    if (res < 0) {
        goto out;
    }

    paging_map_to(
        process->task->page_directory, (void *)USER_STACK_END_VADDR, paddr,
        paging_align_address((void *)((uint32_t)paddr + USER_STACK_SIZE - 1)),
        PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITEABLE);

    // adjust address
    paging_adjust_pd(process->task->page_directory);
out:
    return res;
}

int process_get_free_slot() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i] == 0) {
            return i;
        }
    }

    return -EISTKN;
}

int process_load(const char *filename, struct process **process) {
    int res = 0;
    int process_slot = process_get_free_slot();
    if (process_slot < 0) {
        res = -EISTKN;
        goto out;
    }

    res = process_load_for_slot(filename, process, process_slot);
out:
    return res;
}

int process_load_switch(const char *filename, struct process **process) {
    int res = process_load(filename, process);
    if (res == 0) {
        process_switch(*process);
    }

    return res;
}

int process_load_for_slot(const char *filename, struct process **process,
                          int process_slot) {
    int res = 0;
    struct task *task = 0;
    struct process *_process;
    void *program_stack_ptr = 0;

    if (process_get(process_slot) != 0) {
        res = -EISTKN;
        goto out;
    }

    _process = kcalloc(sizeof(struct process));
    if (!_process) {
        res = -ENOMEM;
        goto out;
    }

    process_init(_process);
    res = process_load_data(filename, _process);
    if (res < 0) {
        goto out;
    }

    program_stack_ptr = kcalloc(USER_STACK_SIZE);
    if (!program_stack_ptr) {
        res = -ENOMEM;
        goto out;
    }

    strncpy(_process->filename, filename, sizeof(_process->filename));
    _process->stack = program_stack_ptr;
    _process->id = process_slot;

    // Create a task
    task = task_new(_process);
    if (ERROR_I(task) == 0) {
        res = ERROR_I(task);
        goto out;
    }

    _process->task = task;

    res = process_map_memory(_process);
    if (res < 0) {
        goto out;
    }

    *process = _process;

    // Add the process to the array
    processes[process_slot] = _process;

out:
    if (ISERR(res)) {
        if (_process && _process->task) {
            task_free(_process->task);
        }

        // Free the process data
    }
    return res;
}
