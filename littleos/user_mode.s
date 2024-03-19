%include "constants.inc"

global enter_user_mode

enter_user_mode:
    ; [esp + 16]  ss      ; the stack segment selector we want for user mode
    ; [esp + 12]  esp     ; the user mode stack pointer
    ; [esp +  8]  eflags  ; the control flags we want to use in user mode
    ; [esp +  4]  cs      ; the code segment selector
    ; [esp +  0]  eip     ; the instruction pointer of user mode code to execute

    push USER_DATA_SEGMENT_SELECTOR
    push 0xBFFFFFFB
    pushf             ; Push EFLAGS onto the stack
    or dword [esp], 2 ; Set the 9th bit (IF) to 1 using OR operation
    push USER_CODE_SEGMENT_SELECTOR
    push 0x00000000

; disable_interrupt:
    cli

; load_registers:
    xchg bx, bx
    mov ax, USER_DATA_SEGMENT_SELECTOR
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    iret
