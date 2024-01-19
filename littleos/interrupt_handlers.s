
%macro isr_no_err_stub 1
global isr_stub_%+%1
isr_stub_%+%1:
    push    dword 0                     ; push 0 as error code
    push    dword %1                    ; push the interrupt number
    jmp     common_interrupt_handler    ; jump to the common handler
%endmacro

%macro isr_err_stub 1
global isr_stub_%+%1
isr_stub_%+%1:
    push    dword %1                    ; push the interrupt number
    jmp     common_interrupt_handler    ; jump to the common handler
%endmacro

common_interrupt_handler:               ; the common parts of the generic interrupt handler
    pushad
    xchg bx, bx
    call    interrupt_handler
    popad

    ; restore the esp
    add     esp, 8
    iret

extern interrupt_handler
; Exceptions, please refer to:
; https://wiki.osdev.org/Exceptions
isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14 ; Exceptions Page Fault
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31
; IRQ please refer to
; https://wiki.osdev.org/Interrupts#Standard_ISA_IRQs
isr_no_err_stub 32 ; IRQ0
isr_no_err_stub 33 ; IRQ1 - keyboard

isr_no_err_stub 128 ; for system call (int 0x80)

global isr_stub_table
isr_stub_table:
%assign i 0
%rep    34
    dd isr_stub_%+i ; use DQ instead if targeting 64-bit
%assign i i+1
%endrep
    dd isr_stub_128

