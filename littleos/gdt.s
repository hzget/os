;Load the Global Descriptor Table
;
; Each time the processor wants to access .text, .data or .stack
; segment, it will use cs, ds or ss respectively to index gdt and
; get the segment descriptor and then access that segment.
;
; Gloable Descriptor Table (in our cases)
; -------------------------------------------------------------
; | Address          | index |            content             |
; | GDTR offset + 0  |   0   |            Null                |
; | GDTR offset + 8  |   1   | kernel code segment descriptor |
; | GDTR offset + 16 |   2   | kernel data segment descriptor |
; | GDTR offset + 24 |   3   | user code segment descriptor   |
; | GDTR offset + 32 |   4   | user data segment descriptor   |
;
; (Re)load GDT
;
;   Loading the GDT into the processor is done with the lgdt
;   assembly code instruction, which takes the address of a struct
;   that specifies the start and size of the GDT.
;   (https://wiki.osdev.org/Global_Descriptor_Table)
;
; (Re)load Segment Registers
;
;   Whatever you do with the GDT has no effect on the CPU until
;   you load new Segment Selectors into Segment Registers.
;   For most of these registers, the process is as simple as
;   using MOV instructions, but changing the CS register
;   requires code resembling a jump or call to elsewhere,
;   as this is the only way its value is meant to be changed.
;   (https://wiki.osdev.org/Global_Descriptor_Table)
;

global segments_load_gdt
global segments_load_registers

segments_load_gdt:
    xchg bx, bx
    lgdt [esp + 4]
    ret

segments_load_registers:
    xchg bx, bx
    ; 0x10 - an offset into GDT for the
    ; third (kernel data segment) record.
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ; 0x08 - an offset into GDT for the
    ; second (kernel code segment) record. 
    jmp 0x08:flush_cs

flush_cs:
    ret
