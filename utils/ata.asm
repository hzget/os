; ata.asm
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

ata_read_disk:
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

