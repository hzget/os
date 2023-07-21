; harddisk.asm
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

; prompt info about MBR
	call print

; read loader from hardware
	mov edi, LOADER_BASE_ADDRESS; target memory location
	mov ecx, 2; starting sector in the hardware
	mov bl, 1; num of sectors
	push dx
	call read_disk
	pop dx

; bochs magic breakpoint (also need to enable it in bochsrc)
; used for debugging
	xchg bx, bx

; run loader
	jmp LOADER_BASE_ADDRESS

; ATA PIO mode
; for details, please refer to
;	https://wiki.osdev.org/ATA_PIO_Mode#Addressing_Modes
;
; 0x1F0: 16bits, read/write data
; 0x1F1: check errors of previous command
; 0x1F2: num of sectors
; 0x1F3: starting sector location - 0 ~ 7 bits
; 0x1F4: starting sector location - 8 ~ 15 bits
; 0x1F5: starting sector location - 16 ~ 32 bits
; 0x1F6: 
;	0 ~ 3: starting sec location - 24 ~ 27 bits
;	4: 0 main, 1 slave
;	6: 0 CHS, 1 LBA
;	5, 7: 1s fixed
; 0x1F7: out
;	0xEC: detect hardware
;	0x20: read
;	0x30: write
; 0x1F7: in / 8bits
;	0 ERR
;	3 DRQ data is ready
;	7 BSY busy

read_disk:
	; num of sectors
	mov dx, 0x1f2
	mov al, bl
	out dx, al
	
	inc dx; 0x1f3
	mov al, cl
	out dx, al

	inc dx; 0x1f4
	shr ecx, 8
	mov al, cl
	out dx, al

	inc dx; 0x1f5
	shr ecx, 8
	mov al, cl
	out dx, al

	inc dx; 0x1f6
	shr ecx, 8
	and cl, 0b1111; retain lower 4 bits

	; main disk, LBA mode
	mov al, 0b1110_0000
	or al, cl
	out dx, al

	inc dx; 0x1f7
	mov al, 0x20; read disk
	out dx, al

	xor ecx, ecx
	mov cl, bl

	.read:
		push cx
		call .waits
		call .reads; read a sector
		pop cx
		loop .read

	ret

	.waits:
		mov dx, 0x1f7	
		.check:
			in al, dx
			jmp $+2; jump to next line (a delay just as nop)
			jmp $+2
			jmp $+2
			and al, 0b1000_1000
			cmp al, 0b0000_1000
			jnz .check
		ret

	.reads:
		mov dx, 0x1f0
		mov cx, 256; sec size = 256
		.readw:
			in ax, dx
			jmp $+2
			jmp $+2
			jmp $+2
			mov [edi], ax
			add edi, 2
			loop .readw
		ret

; print message
; please refer to http://www.ctyme.com/intr/rb-0210.htm
print:
	mov ax, message
	mov bp, ax
	mov cx, 17
	mov ax, 0x1301
	; character color is set to red
	; please refer to http://www.ctyme.com/intr/rb-0098.htm#Table15
	mov bx, 0x4
    ; reset DH, DL to begin from the 1st position
    mov dx, 0
	int 0x10
	ret

	message db "MBR is running..."

; fill in with 0s
	times 510 - ($-$$) db 0

; last two bytes
	db 0x55, 0xaa
