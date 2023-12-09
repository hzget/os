;
; pmode.asm
;
;   It illustrates how to switch from real mode
;   to protected mode and run code in it.
;
;   It is modified from the following code:
;     https://huichen-cs.github.io/course/CISC3320/19FA/lecture/modeswitch.html
;
;   1. print a string in real mode with bios function
;   2. switch to protected mode and init registers
;   3. print a string in protected mode
;
[org 0x7c00]

; set up real-mode stack
mov bp, 0x9000  
mov sp, bp

; bochs magic breakpoint (also need to enable it in bochsrc)
; used for debugging
xchg bx, bx

; print a message in real mode
mov bx, MSG_REAL_MODE
call print_msg_rm

; used for debugging
xchg bx, bx

; switch to protected mode
call switch_to_pm

; run code in protected mode
call main

; we will never return here if the above is successful
jmp $

%include "utils/gdt.asm"
%include "utils/print_msg_rm.asm"
%include "utils/print_msg_pm.asm"
%include "utils/switch_to_pm.asm"

[bits 32]
main:
    ; do something useful, such as, print a message in protected mode
    mov ebx, MSG_PROT_MODE
    call print_msg_pm

    jmp $

MSG_REAL_MODE:
     db "Started in 16-bit real mode", 0

MSG_PROT_MODE:
    db "Switched to 32-bit protected mode", 0

times 510-($-$$) db 0
dw 0xaa55
