#include "constants.h"
#include "multiboot.h"
#include "paging.h"
#include "stdio.h"
#include "string.h"
#include <stdint.h>

extern uint32_t grub_multiboot_info;
extern uint32_t kernel_pd;
extern void enter_user_mode();

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
        printf("no apps to run \n");
        return;
    }

    multiboot_module_t *module = (multiboot_module_t *)mbi->mods_addr;
    module = (multiboot_module_t *)((uint32_t)module + KERNEL_START_VADDR);
    uint32_t size = module->mod_end - module->mod_start;
    uint32_t addr = module->mod_start;
    typedef void (*call_module_t)(void);
    addr += KERNEL_START_VADDR;
    printf("module start addr 0x%x, size 0x%x\n", addr, size);

    paging_switch(create_user_pd());

    uint32_t *newaddr = USER_CODE_VADDR;
    memcpy((uint8_t *)newaddr, (uint8_t *)addr, size);

    enter_user_mode();

    asm volatile("xchgw %bx, %bx");

    // If we use newaddr directly, gcc -Werror=pedantic
    // will give an error:
    // error: ISO C forbids conversion of object pointer to function pointer
    // type [-Werror=pedantic]
    // call_module_t start_program = (call_module_t)(newaddr);
    call_module_t start_program = (call_module_t)(USER_CODE_VADDR);
    start_program();
    paging_switch(&kernel_pd);
}
