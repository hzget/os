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

static int task_init(struct task *task, struct process *process);

struct task *task_current() {
    return current_task;
}

struct task *task_new(struct process *process) {
    int res = 0;
    struct task *task = kcalloc(sizeof(struct task));
    if (!task) {
        res = -ENOMEM;
        goto out;
    }

    res = task_init(task, process);
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
    if (!current_task) {
        return NULL;
    }
    if (!current_task->next) {
        return task_head;
    }

    return current_task->next;
}

void task_next() {
    int res = 0;
    struct task *next_task = task_get_next();
    if (!next_task) {
        printf("No more tasks!\n");
        return;
    }

    if (next_task == current_task) {
        // do nothing
        return;
    }
    res = task_switch(next_task);
    if (res < 0) {
        printf("memory not mapped!\n");
        return;
    }
    task_return(&next_task->registers);
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
    if (!task->mmapped) {
        return -1;
    }
    current_task = task;
    paging_switch(task->page_directory);
    return 0;
}

void task_run_first_ever_task() {
    if (!current_task) {
        PANIC("task_run_first_ever_task(): No current task exists!\n");
    }

    int res = task_switch(task_head);
    if (res < 0) {
        return;
    }

    task_return(&task_head->registers);
}

static int task_init(struct task *task, struct process *process) {
    memset(task, 0, sizeof(struct task));

    task->page_directory = paging_new_directory();
    if (!task->page_directory) {
        return -EIO;
    }

    task->registers.ip = USER_CODE_VADDR;
    task->registers.cs = USER_CODE_SEGMENT_SELECTOR;
    task->registers.esp = USER_STACK_VADDR;
    task->registers.ss = USER_DATA_SEGMENT_SELECTOR;

    task->process = process;

    return 0;
}

void task_save_state(struct task *task, struct interrupt_frame *frame) {
    task->registers.ip = frame->ip;
    task->registers.cs = frame->cs;
    task->registers.flags = frame->flags;
    task->registers.esp = frame->esp;
    task->registers.ss = frame->ss;
    task->registers.eax = frame->eax;
    task->registers.ebp = frame->ebp;
    task->registers.ebx = frame->ebx;
    task->registers.ecx = frame->ecx;
    task->registers.edi = frame->edi;
    task->registers.edx = frame->edx;
    task->registers.esi = frame->esi;
}

void task_current_save_state(struct interrupt_frame *frame) {
    if (!task_current()) {
        PANIC("No current task to save\n");
    }

    struct task *task = task_current();
    task_save_state(task, frame);
}

void *task_get_stack_item(struct task *task, int index) {
    void *result = 0;
    uint32_t *sp_ptr = (uint32_t *)task->registers.esp;

    result = (void *)sp_ptr[index];
    return result;
}
