ENTRY(loader)                /* the name of the entry label */

SECTIONS {
    . = 0xC0100000;          /* the code should be relocated to 3GB + 1MB */

    kernel_virtual_start = .;
    kernel_physical_start = . - 0xC0000000;
    /* align at 4 KB and load at 1 MB (physical address) */
    .text ALIGN (0x1000) :   AT(ADDR(.text)-0xC0000000)
    {
        *(.text)             /* all text sections from all files */
    }

    .rodata ALIGN (0x1000) : AT(ADDR(.rodata)-0xC0000000)
    {
        *(.rodata*)          /* all read-only data sections from all files */
    }

    .data ALIGN (0x1000) :   AT(ADDR(.data)-0xC0000000)
    {
        *(.data)             /* all data sections from all files */
    }

    .bss ALIGN (0x1000) :    AT(ADDR(.bss)-0xC0000000)
    {
        *(COMMON)            /* all COMMON sections from all files */
        *(.bss)              /* all bss sections from all files */
    }

    .kernel_heap_section : {
        /* Reserve some bytes in the kernel_heap_section */
        kernel_heap_virtual_address = .;
        . = kernel_heap_virtual_address + 0x1000 * 200;
    }

    kernel_virtual_end = .;
    kernel_physical_end = . - 0xC0000000;
}
