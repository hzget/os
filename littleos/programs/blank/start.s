[bits 32]

section .text
_start:
    
    push 30
    push 20
    mov eax, 0
    int 0x80
    add esp, 8
    jmp  $    ; loop forever

