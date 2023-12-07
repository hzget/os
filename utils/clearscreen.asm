

; clear screen
clearscreen:
	mov ax, 0x600
	mov bx, 0x700
	mov cx, 0
	mov dx, 0x184f
	int 0x10
	ret

