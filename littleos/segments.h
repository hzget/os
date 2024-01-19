/** @file */
#ifndef SEGMENTS_H
#define SEGMENTS_H

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
void init_gdt();

struct GDTR {
    unsigned short size;
    unsigned int address;
} __attribute__((packed));

struct GDT {
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char access_byte;
    unsigned char limit_and_flags;
    unsigned char base_high;
} __attribute__((packed));

// Wrappers around ASM.
void segments_load_gdt(struct GDTR gdt);
void segments_load_registers();
#endif /* SEGMENTS_H */
