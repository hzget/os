/** @file */

/*
 * For the definition of GDB, pls refer to:
 *
 *   https://wiki.osdev.org/Global_Descriptor_Table
 *
 * For TSS entry in GDB , pls refer to:
 *
 *   https://wiki.osdev.org/Task_State_Segment#TSS_in_software_multitasking
 *
 */

#include "segments.h"
#include "stdio.h"
#include "tss.h"

#define TSS_SELECTOR (5 * 8)

#define BASE 0
#define LIMIT 0xFFFFF

/* Bit:     | 7 | 65 | 4 | 3 | 2 | 1 | 0 |
 * Content: | P | DPL| S | E | DC| RW| A |
 * Value:   | 1 | 00 | 1 | 1 | 0 | 0 | 1 | = 0x9A
 * Value:   | 1 | 00 | 1 | 0 | 0 | 1 | 0 | = 0x92
 * Value:   | 1 | 11 | 1 | 1 | 0 | 0 | 1 | = 0xFA
 * Value:   | 1 | 11 | 1 | 0 | 0 | 1 | 0 | = 0xF2
 */
#define CODE_ACCESS_BYTE 0x9A
#define DATA_ACCESS_BYTE 0x92
#define USER_CODE_ACCESS_BYTE 0xFA
#define USER_DATA_ACCESS_BYTE 0xF2

/* Bit:     | 7 | 65 | 4 | 3210 |
 * Content: | P | DPL| S | TYPE |
 * Value:   | 1 | 00 | 0 | 1001 | = 0x89
 */
#define TSS_ACCESS_BYTE 0x89

/*
 * Flags part of `limit_and_flags`.
 * 1100
 * 0 - Available for system use
 * 0 - Long mode
 * 1 - Size (0 for 16-bit, 1 for 32)
 * 1 - Granularity (0 for 1B - 1MB, 1 for 4KB - 4GB)
 */
#define FLAGS 0x0C
#define TSS_FLAGS 0x0

static struct GDT gdt[6];

static void init_gdt_entry(int index, uint32_t base_address, uint32_t limit,
                           uint8_t access_byte, uint8_t flags) {

    gdt[index].base_low = base_address & 0xFFFF;
    gdt[index].base_middle = (base_address >> 16) & 0xFF;
    gdt[index].base_high = (base_address >> 24) & 0xFF;

    gdt[index].limit_low = limit & 0xFFFF;
    gdt[index].limit_and_flags = (limit >> 16) & 0xF;
    gdt[index].limit_and_flags |= (flags << 4) & 0xF0;

    gdt[index].access_byte = access_byte;
}

/** init_gdt:
 *  create a global descriptor table (GDT) containing
 *  .text and .data segemnt desciptors and then
 *  update gdtr and segment registors to use the
 *  new gdt.
 *
 *  gdt index:
 *    0 : null descriptor
 *    1 : kernel code segment descriptor
 *    2 : kernel data segment descriptor
 *    3 : user code segment descriptor
 *    4 : user data segment descriptor
 *    5 : tss descriptor
 */
void init_gdt() {

    struct GDTR gdtr;
    gdtr.address = (uint32_t)gdt;
    gdtr.size = sizeof(gdt) - 1;

    init_gdt_entry(0, 0x0, 0x0, 0x0, 0x0);
    init_gdt_entry(1, BASE, LIMIT, CODE_ACCESS_BYTE, FLAGS);
    init_gdt_entry(2, BASE, LIMIT, DATA_ACCESS_BYTE, FLAGS);
    init_gdt_entry(3, BASE, LIMIT, USER_CODE_ACCESS_BYTE, FLAGS);
    init_gdt_entry(4, BASE, LIMIT, USER_DATA_ACCESS_BYTE, FLAGS);
    init_gdt_entry(5, tss_addr(), sizeof(tss_t), TSS_ACCESS_BYTE, TSS_FLAGS);

    segments_load_gdt(gdtr);
    segments_load_registers();
    printf("gdt installed\n");

    tss_load_and_set(TSS_SELECTOR);
}
