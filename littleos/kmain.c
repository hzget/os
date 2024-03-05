/** @file */

#include "check.h"
#include "constants.h"
#include "disk.h"
#include "framebuffer.h"
#include "interrupts.h"
#include "keyboard.h"
#include "kheap.h"
#include "module.h"
#include "paging.h"
#include "segments.h"
#include "serial.h"
#include "stdio.h"
#include "syscall.h"
#include "tss.h"

char welcome[] = "Little OS";

void kmain() {
    fb_clear();
    printf("Hello %s\n", welcome);
    check_multiboot();
    check_kernel();
    // serial_write(welcome, sizeof(welcome));
    init_kb();
    init_gdt();
    init_idt();
    init_paging();
    // check_page_fault();
    init_kheap();
    // check_kheap();
    init_fs();
    init_disk();
    check_disk();
    init_tss();
    init_syscalls();
    check_task();
    // check_process();
    run_apps();
    check_pparser();
    check_streamer();
    check_fopen();
}
