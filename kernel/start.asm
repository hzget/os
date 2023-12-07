; kernel.asm
;
;   It just illustrates how to run into kernel from loader
;

extern kernel_init

global _start
_start:
    call kernel_init
    jmp $

