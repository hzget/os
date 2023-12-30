#ifndef CONSTANTS_H
#define CONSTANTS_H

/* numeric contants */
#define _1KB 0x400
#define _4KB 0x1000
#define _1MB 0x100000

/* virtual memory */
/* size: */
/*     pd 1024 entries, pt 1024 entries, pf 4*1024 bytes, */
/*     memory = 1024*1024*4096 = 4G bytes */
/* pd entry index for pf starts at 0xC0000000 will be */
/*     0xC0000000 >> 22 */
#define KERNEL_START_VADDR 0xC0000000
#define KERNEL_PDT_IDX (KERNEL_START_VADDR >> 22)

#define PAGE_FRAME_SIZE _4KB

/* kernel stack */
#define KERNEL_STACK_SIZE _4KB

#endif
