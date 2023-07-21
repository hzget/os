; graphics.asm
;    
;   It is a tutorial program in MBR.
;   It just write a string "2 MBR" directly to the Memory of Graphics Card
;   in the address from 0x8000
;
;	BIOS interrupt involved: 0x10
;   please refer to
;		https://grandidierite.github.io/bios-interrupts/
;		http://www.ctyme.com/intr/int-10.htm
;
SECTION MBR vstart=0x7c00

	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov fs, ax
	mov sp, 0x7c00
	mov ax, 0xb800
	mov gs, ax

; clear screen
	mov ax, 0x600
	mov bx, 0x700
	mov cx, 0
	mov dx, 0x184f
	int 0x10

; set cursor position
	mov ah, 0x02
	mov bh, 0
	mov dx, 0 ; upper left corner
	int 0x10

; writing a string "2 MBR" to mem of Graphics Card
	mov byte [gs:0x00], '2'
	mov byte [gs:0x01], 0xA4

	mov byte [gs:0x02], ' '
	mov byte [gs:0x03], 0xA4

	mov byte [gs:0x04], 'M'
	mov byte [gs:0x05], 0xA4

	mov byte [gs:0x06], 'B'
	mov byte [gs:0x07], 0xA4

	mov byte [gs:0x08], 'R'
	mov byte [gs:0x09], 0xA4

; block here
	jmp $

; fill in with 0s
	times 510 - ($-$$) db 0

; last two bytes
	db 0x55, 0xaa
