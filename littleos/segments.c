/** @file */
#include "segments.h"
#include "stdio.h"

#define DESCRIPTORS_COUNT 3

#define BASE 0
#define LIMIT 0xFFFFF

#define CODE_TYPE 0x9A
#define DATA_TYPE 0x92

/*
 * Flags part of `limit_and_flags`.
 * 1100
 * 0 - Available for system use
 * 0 - Long mode
 * 1 - Size (0 for 16-bit, 1 for 32)
 * 1 - Granularity (0 for 1B - 1MB, 1 for 4KB - 4GB)
 */
#define FLAGS_PART 0x0C

static struct GDTDescriptor gdt_descriptors[DESCRIPTORS_COUNT];

static void segments_init_descriptor(int index, unsigned int base_address,
                                     unsigned int limit,
                                     unsigned char access_byte,
                                     unsigned char flags) {

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
    gdt.address = (unsigned int)gdt_descriptors;
    gdt.size = (sizeof(struct GDTDescriptor) * DESCRIPTORS_COUNT) - 1;

    segments_init_descriptor(0, 0x0, 0x0, 0x0, 0x0);
    segments_init_descriptor(1, BASE, LIMIT, CODE_TYPE, FLAGS_PART);
    segments_init_descriptor(2, BASE, LIMIT, DATA_TYPE, FLAGS_PART);

    segments_load_gdt(gdt);
    segments_load_registers();
    printf("gdt installed\n");
}
