#include "stdint.h"
#include "stdio.h"
#include "multiboot.h"
#include "constants.h"

extern uint32_t grub_multiboot_info;

void create_pageframes() {
}

void create_pt() {
}

void copy_module_code() {
}

void user_mod_init() {
}

void run_apps() {
    static multiboot_info_t *mbi;
    mbi = (multiboot_info_t *)(grub_multiboot_info + KERNEL_START_VADDR);
    if (!(mbi->flags & MULTIBOOT_INFO_MODS)) {
        printf("no apps to run\n");
        return;
    }

    multiboot_module_t *module = (multiboot_module_t *)mbi->mods_addr;
    module = (multiboot_module_t *)((uint32_t)module + KERNEL_START_VADDR);
    uint32_t size = module->mod_end - module->mod_start;
    uint32_t addr = module->mod_start;
    typedef void (*call_module_t)(void);
    addr += KERNEL_START_VADDR;
    printf("module start addr %x, size %x\n", addr, size);
    call_module_t start_program = (call_module_t)(addr);
    start_program();
}
