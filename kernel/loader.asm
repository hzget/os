; loader.asm
;
; It is a loader that resides the hardware.
; It just load kernel from harddisk and then switch to run the kernel
;
; Note: before a MBR reads the loader, it shall exist
; in the harddisk. How to make an image containing it ?
; Just suppose MBR is in 1st sector, now we will write
; the loader to the 3rd sector:
;
;		nasm loader.asm -o loader.bin
;		dd if=loader.bin of=master.img seek=2
;
; (master.img is the image file)
;

%include "boot.inc"
section loader vstart=LOADER_BASE_ADDRESS

; prompt info about Loader
mov bx, MSG_LOADER_REAL_MODE
call print_msg_rm

; used for debugging
xchg bx, bx

; switch to protected mode
call switch_to_pm

; used for debugging
xchg bx, bx

%include "../utils/gdt.asm"
%include "../utils/print_msg_rm.asm"
%include "../utils/print_msg_pm.asm"
%include "../utils/switch_to_pm.asm"

[bits 32]
mov ebx, MSG_LOADER_PROTECTED_MODE
call print_msg_pm

; used for debugging
xchg bx, bx

; load kernel
; KERNEL_ADDRESS equ 0x10000

mov edi, KERNEL_ADDRESS; target memory location
mov ecx, 10; starting sector in the hardware
mov bl, 200; num of sectors
call ata_read_disk

; used for debugging
xchg bx, bx

; jump to kernel
jmp KERNEL_ADDRESS

; block here
jmp $

%include "../utils/ata.asm"

MSG_LOADER_REAL_MODE db "loader is running in real mode now...", 0xA, 0xD, 0
MSG_LOADER_PROTECTED_MODE db "loader is running in protected mode now ...", 0

