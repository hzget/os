; mbr.asm
;
; It's a MBR program resides the hardware.
;
; It just illustrates how a MBR program reads a loader
; from hardware and run that loader.
;
; It makes use of ATA PIO mode to transfer data:
;	Every byte of data transferred between the disk and the CPU
;	must be sent through the CPU's IO port bus (not the memory).
;	please refer to https://wiki.osdev.org/ATA_PIO_Mode
;

%include "boot.inc"
SECTION MBR vstart=0x7c00

	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov fs, ax
	mov sp, 0x7c00
	mov ax, 0xb800
	mov gs, ax

	call clearscreen

; set cursor position
	mov ah, 0x02
	mov bh, 0
	mov dx, 0 ; upper left corner
	int 0x10

	mov bx, MSG_MBR_REAL_MODE
	call print_msg_rm

; used for debugging
	xchg bx, bx

; read loader from hardware
	mov edi, LOADER_BASE_ADDRESS; target memory location
	mov ecx, 2; starting sector in the hardware
	mov bl, 1; num of sectors
	push dx
	call ata_read_disk
	pop dx

; bochs magic breakpoint (also need to enable it in bochsrc)
; used for debugging
	xchg bx, bx

; run loader
	jmp LOADER_BASE_ADDRESS

jmp $

%include "../utils/ata.asm"
%include "../utils/clearscreen.asm"
%include "../utils/print_msg_rm.asm"
MSG_MBR_REAL_MODE db "MBR is running...", 0xA, 0xD, 0

; fill in with 0s
times 510 - ($-$$) db 0

; last two bytes
db 0x55, 0xaa
