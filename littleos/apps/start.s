extern main

section .text
    ; push argv
    ; push argc
    push ebp
    mov ebp, esp
	xchg bx, bx
    call main
	xchg bx, bx
    ; main has returned, eax is return value
    jmp  $    ; loop forever
    pop ebp
    ret

