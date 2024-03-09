#include "check.h"
#include "constants.h"
#include "disk.h"
#include "kheap.h"
#include "log.h"
#include "multiboot.h"
#include "pit.h"
#include "pparser.h"
#include "process.h"
#include "stdio.h"
#include "streamer.h"
#include "string.h"
#include "task.h"
#include <stdint.h>

extern uint32_t grub_multiboot_info;
extern uint32_t kernel_physical_start;
extern uint32_t kernel_physical_end;
extern uint32_t kernel_virtual_start;
extern uint32_t kernel_virtual_end;

static multiboot_info_t *mbi;

static void mbi_check_overview();
static void mbi_check_mmap();
static void mbi_check_init();

void check_page_fault() {
    // normal
    uint32_t *ptr = (uint32_t *)0xC0100000;
    printf("[0x%08x] is 0x%08x\n", ptr, *ptr);

    // fault
    ptr = (uint32_t *)0x001E0000;
    printf("[0x%08x] is 0x%08x\n", ptr, *ptr);
}

void check_address_access(uint32_t *addr) {
    printf("%s: 0x%08x[0]=0x%08x\n", __func__, addr, addr[0]);
}

void check_process() {
    log_debug("check", "enter %s()\n", __func__);
    struct process *process;
    int res = process_load_switch("1:/sayhello.bin", &process);
    if (res < 0) {
        printf("%s: res == %d\n", __func__, res);
        goto out;
    }

    res = process_load_switch("1:/saysorry.bin", &process);
    if (res < 0) {
        printf("%s: res == %d\n", __func__, res);
        goto out;
    }

    task_run_first_ever_task();

out:
    // how to free the process?

    log_debug("check", "leaving %s()\n", __func__);
}

void check_task() {
    struct task *t = task_new(NULL);
    log_debug("check_task", "task pd=0x%x\n", t->page_directory);
    task_free(t);
}

void check_disk() {
    char buf[512];
    int32_t status = disk_read_block(disk_get(1), 0, 1, buf);
    if (!SUCCESS(status)) {
        printf("%s() failed: status is %d\n", __func__, status);
    }
    asm volatile("xchgw %bx, %bx");
}

void check_pparser() {
    path_root_t *root = pathparser_parse("1:/proj/github.com");
    pathparser_free(root);
}

void check_streamer() {
    disk_stream_t *stream = diskstreamer_new(1);
    diskstreamer_seek(stream, 0x100);
    void *out = kcalloc(16);
    int32_t status = diskstreamer_read(stream, out, 16);
    if (!SUCCESS(status)) {
        printf("%s() failed, status: %d\n", __func__, status);
    } else {
        printf("%s() out = ", __func__);
        for (uint32_t i = 0; i < 16; i++) {
            printf("0x%x ", ((uint8_t *)out)[i]);
        }
        printf("\n");
    }
    diskstreamer_close(stream);
    kfree(out);
}

void check_fopen() {
    int fd = fopen("1:/hello.txt", "r");
    printf("%s: fopen 1:/hello.txt = %d\n", __func__, fd);
    if (fd > 0) {
        char buf[12];
        memset((uint8_t *)buf, 0, sizeof(buf));
        int n = fread(buf, 1, 9, fd);
        printf("fread return %d bytes: %s\n", n, buf);
        memset((uint8_t *)buf, 0, sizeof(buf));
        n = fseek(fd, 2, SEEK_SET);
        n = fread(buf, 1, 7, fd);
        printf("fread return %d bytes: %s\n", n, buf);
        struct file_stat stat;
        n = fstat(fd, &stat);
        printf("fstat: flags: %u, filesize: %d\n", stat.flags, stat.filesize);
        fclose(fd);
    }
}

void check_kheap() {
    void *ptr1 = kmalloc(4096);
    printf("ptr1: %x\n", ptr1);
    void *ptr2 = kmalloc(4097);
    printf("ptr2: %x\n", ptr2);
    kheap_print_table_entries(8);
    void *ptr3 = kmalloc(4096);
    printf("ptr3: %x\n", ptr3);
    kheap_print_table_entries(8);
    kfree(ptr2);
    kheap_print_table_entries(8);
    void *ptr4 = kmalloc(4096);
    printf("ptr4: %x\n", ptr4);
    kheap_print_table_entries(8);
    void *ptr5 = kmalloc(4097);
    printf("ptr5: %x\n", ptr5);
    kheap_print_table_entries(8);
    kfree((uint32_t *)ptr5 + 0x30);
    kheap_print_table_entries(8);
}

void check_kernel() {
    uint32_t _pstart = (uint32_t)&kernel_physical_start;
    uint32_t _pend = (uint32_t)&kernel_physical_end;
    uint32_t _vstart = (uint32_t)&kernel_virtual_start;
    uint32_t _vend = (uint32_t)&kernel_virtual_end;
    printf("Kernel Address:\n"
           "physical addr 0x%08x ~ 0x%08x, size 0x%08x\n"
           "virtual  addr 0x%08x ~ 0x%08x, size 0x%08x\n",
           _pstart, _pend, _pend - _pstart, _vstart, _vend, _vend - _vstart);
}

void check_multiboot() {
    mbi_check_init();
    mbi_check_overview();
    mbi_check_mmap();
}

static void mbi_check_init() {
    mbi = (multiboot_info_t *)(grub_multiboot_info + KERNEL_START_VADDR);
}

static void mbi_check_overview() {
    printf("multiboot info addr is 0x%08x, flag is 0x%08x\n", mbi, mbi->flags);
}

static void mbi_check_mmap() {
    if (!(mbi->flags & MULTIBOOT_INFO_MEM_MAP)) {
        printf("no memory map given by GRUB bootloader");
        return;
    }

    printf("memory map given by GRUB bootloader:\n");
    uint32_t i;
    for (i = 0; i < mbi->mmap_length; i += sizeof(multiboot_memory_map_t)) {
        multiboot_memory_map_t *mmmt =
            (multiboot_memory_map_t *)(mbi->mmap_addr + i + KERNEL_START_VADDR);
        /* An excerpt of the output:
         *
         *     Start Addr: 0x00000000 | Length: 0x0009FC00 | Size: 20 | Type: 1
         *     Start Addr: 0x0009FC00 | Length: 0x00000400 | Size: 20 | Type: 2
         *     Start Addr: 0x000F0000 | Length: 0x00010000 | Size: 20 | Type: 2
         *     Start Addr: 0x00100000 | Length: 0x002E0000 | Size: 20 | Type: 1
         *     Start Addr: 0x003E0000 | Length: 0x00020000 | Size: 20 | Type: 2
         *     Start Addr: 0xFFFC0000 | Length: 0x00040000 | Size: 20 | Type: 2
         */
        printf("Start Addr: 0x%08x | Length: 0x%08x | Size: %u | Type: %u\n",
               mmmt->addr_low, mmmt->len_low, mmmt->size, mmmt->type);

        if (mmmt->type == MULTIBOOT_MEMORY_AVAILABLE) {
            /*
             * Do something with this memory block!
             * BE WARNED that some of memory shown as availiable is actually
             * actively being used by the kernel! You'll need to take that
             * into account before writing to memory!
             */
        }
    }
}
