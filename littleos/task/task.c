#include "task.h"
#include "check.h"
#include "constants.h"
#include "kheap.h"
#include "paging.h"
#include "status.h"
#include "stdio.h"
#include "string.h"

struct task *current_task = 0;

struct task *task_tail = 0;
struct task *task_head = 0;

static int task_init(struct task *task);

struct task *task_current() {
    return current_task;
}

struct task *task_new() {
    int res = 0;
    struct task *task = kcalloc(sizeof(struct task));
    if (!task) {
        res = -ENOMEM;
        goto out;
    }

    res = task_init(task);
    if (res != STATUS_OK) {
        goto out;
    }

    if (task_head == 0) {
        task_head = task;
        task_tail = task;
        current_task = task;
        goto out;
    }

    task_tail->next = task;
    task->prev = task_tail;
    task_tail = task;

out:
    if (ISERR(res)) {
        task_free(task);
        return ERROR(res);
    }

    return task;
}

struct task *task_get_next() {
    if (!current_task->next) {
        return task_head;
    }

    return current_task->next;
}

static void task_list_remove(struct task *task) {
    if (task->prev) {
        task->prev->next = task->next;
    }

    if (task == task_head) {
        task_head = task->next;
    }

    if (task == task_tail) {
        task_tail = task->prev;
    }

    if (task == current_task) {
        current_task = task_get_next();
    }
}

int task_free(struct task *task) {
    paging_free(task->page_directory);
    task_list_remove(task);

    kfree(task);
    return 0;
}

int task_switch(struct task *task) {
    current_task = task;
    paging_switch(task->page_directory);
    return 0;
}

static int task_init(struct task *task) {
    memset(task, 0, sizeof(struct task));

    task->page_directory = create_user_pd();
    if (!task->page_directory) {
        return -EIO;
    }

    task->registers.ip = USER_CODE_VADDR;
    task->registers.cs = USER_CODE_SEGMENT_SELECTOR;
    task->registers.esp = USER_STACK_VADDR;
    task->registers.ss = USER_DATA_SEGMENT_SELECTOR;

    return 0;
}
