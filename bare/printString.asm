; printString.asm
;
;   It is a tutorial program in MBR.
;   It just triggers a BIOS interrupt function (0x10)
;   to display the string "MBR is running" on the screen.
;
; The interrupt info can check the following site:
;	http://www.ctyme.com/intr/rb-0210.htm
; character color:
;	http://www.ctyme.com/intr/rb-0098.htm#Table15
;
SECTION MBR vstart=0x7c00

	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov fs, ax
	mov sp, 0x7c00

; clear screen
	mov ax, 0x600
	mov bx, 0x700
	mov cx, 0
	mov dx, 0x184f
	int 0x10

; print message
; FROM http://www.ctyme.com/intr/rb-0210.htm
;
;   VIDEO - WRITE STRING (AT and later,EGA)
;
;   AH = 13h
;   AL = write mode
;   bit 0:  Update cursor after writing
;   bit 1:  String contains alternating characters and attributes
;   bits 2-7:  Reserved (0).
;
;   BH = page number.
;   BL = attribute if string contains only characters.
;   CX = number of characters in string.
;   DH,DL = row,column at which to start writing.
;   ES:BP -> string to write

	mov ax, message
	mov bp, ax
	mov cx, 14
	mov ax, 0x1301
;	character color is set to red
;	please refer to http://www.ctyme.com/intr/rb-0098.htm#Table15
	mov bx, 0x4
	; reset DH, DL to begin from the 1st position
	mov dx, 0
	int 0x10

; block here
	jmp $

	message db "MBR is running"

; fill in with 0s
	times 510 - ($-$$) db 0

; last two bytes
	db 0x55, 0xaa
