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

#define DESCRIPTORS_COUNT 6

#define TSS_SEGSEL (5 * 8)

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

static struct GDTDescriptor gdt_descriptors[DESCRIPTORS_COUNT];

static void segments_init_descriptor(int index, uint32_t base_address,
                                     uint32_t limit, uint8_t access_byte,
                                     uint8_t flags) {

    gdt_descriptors[index].base_low = base_address & 0xFFFF;
    gdt_descriptors[index].base_middle = (base_address >> 16) & 0xFF;
    gdt_descriptors[index].base_high = (base_address >> 24) & 0xFF;

    gdt_descriptors[index].limit_low = limit & 0xFFFF;
    gdt_descriptors[index].limit_and_flags = (limit >> 16) & 0xF;
    gdt_descriptors[index].limit_and_flags |= (flags << 4) & 0xF0;

    gdt_descriptors[index].access_byte = access_byte;
}

/** init_gdt:
 *  create a global descriptor table (GDT) containing
 *  .text and .data segemnt desciptors and then
 *  update gdtr and segment registors to use the
 *  new gdt.
 *
 *  gdt index:
 *    0 : null descriptor
 *    1 : code segement descriptor
 *    2 : data segement descriptor
 */
void init_gdt() {

    struct GDT gdt;
    gdt.address = (uint32_t)gdt_descriptors;
    gdt.size = (sizeof(struct GDTDescriptor) * DESCRIPTORS_COUNT) - 1;

    segments_init_descriptor(0, 0x0, 0x0, 0x0, 0x0);
    segments_init_descriptor(1, BASE, LIMIT, CODE_ACCESS_BYTE, FLAGS);
    segments_init_descriptor(2, BASE, LIMIT, DATA_ACCESS_BYTE, FLAGS);
    segments_init_descriptor(3, BASE, LIMIT, USER_CODE_ACCESS_BYTE, FLAGS);
    segments_init_descriptor(4, BASE, LIMIT, USER_DATA_ACCESS_BYTE, FLAGS);
    segments_init_descriptor(5, tss_addr(), sizeof(tss_t) - 1, TSS_ACCESS_BYTE,
                             TSS_FLAGS);

    segments_load_gdt(gdt);
    segments_load_registers();
    printf("gdt installed\n");

    tss_load_and_set(TSS_SEGSEL);
}
