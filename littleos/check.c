#include "constants.h"
#include "multiboot.h"
#include "stdint.h"
#include "stdio.h"

extern uint32_t grub_multiboot_info;
extern uint32_t kernel_physical_start;
extern uint32_t kernel_physical_end;
extern uint32_t kernel_virtual_start;
extern uint32_t kernel_virtual_end;

static multiboot_info_t *mbi;

static void mbi_check_overview();
static void mbi_check_mmap();
static void mbi_check_init();

void page_fault_check() {
    // normal
    uint32_t *ptr = (uint32_t *)0xC0100000;
    printf("[%x] is %x\n", ptr, *ptr);

    // fault
    ptr = (uint32_t *)0x001E0000;
    printf("[%x] is %x\n", ptr, *ptr);
}

void kernel_check() {
    uint32_t _pstart = (uint32_t)&kernel_physical_start;
    uint32_t _pend = (uint32_t)&kernel_physical_end;
    uint32_t _vstart = (uint32_t)&kernel_virtual_start;
    uint32_t _vend = (uint32_t)&kernel_virtual_end;
    printf("Kernel Address:\n"
           "physical addr %x ~ %x, size %x\n"
           "virtual  addr %x ~ %x, size %x\n",
           _pstart, _pend, _pend - _pstart, _vstart, _vend, _vend - _vstart);
}

void multiboot_check() {
    mbi_check_init();
    mbi_check_overview();
    mbi_check_mmap();
}

static void mbi_check_init() {
    mbi = (multiboot_info_t *)(grub_multiboot_info + KERNEL_START_VADDR);
}

static void mbi_check_overview() {
    printf("multiboot info addr is %x, flag is %x\n", mbi, mbi->flags);
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
        printf("Start Addr: %x | Length: %x | Size: %u | Type: %u\n",
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
