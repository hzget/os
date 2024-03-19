[bits 32]

section .text
_start:
    
print_msg:
    push message
    mov eax, 1   ; command print
    int 0x80
    add esp, 4
    jmp print_msg

    jmp  $    ; loop forever

section .data
message: db 'Sorry', 0
