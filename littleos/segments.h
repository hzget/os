/** @file */
#ifndef INCLUDE_SEGMENTS
#define INCLUDE_SEGMENTS

/** segments_install_gdt:
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
void segments_install_gdt();

struct GDT {
    unsigned short size;
    unsigned int address;
} __attribute__((packed));

struct GDTDescriptor {
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access_byte;
    unsigned char limit_and_flags;
    unsigned char base_high;
} __attribute__((packed));

// Wrappers around ASM.
void segments_load_gdt(struct GDT gdt);
void segments_load_registers();
#endif /* INCLUDE_MEMORY_SEGMENTS */
