; toyloader.asm
;
; It is a toy loader that resides the hardware.
; It just prints a string on the screen.
;
; It is used for illustrating how
; a MBR program loads a loader and runs it.
;
; Note: before a MBR reads the loader, it shall exist
; in the harddisk. How to make an image containing it ?
; Just suppose MBR is in 1st sector, now we will write
; the loader to the 3rd sector:
;
;		nasm toyloader.asm -o toyloader.bin
;		dd if=toyloader.bin of=master.img seek=2
;
; (master.img is the image file)

%include "boot.inc"
section loader vstart=LOADER_BASE_ADDRESS

; print message
; please refer to http://www.ctyme.com/intr/rb-0210.htm
	mov ax, message
	mov bp, ax
	mov cx, 24
	mov ax, 0x1301
	mov bx, 0x4
	add dh, 2 ; print the string on the 3rd line
	int 0x10

; block here
	jmp $

	message db "loader is running now..."