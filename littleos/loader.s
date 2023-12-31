
%include "constants.inc"

global loader                   ; the entry symbol for ELF
extern kmain
extern kernel_physical_end

; setting up the multiboot headers for GRUB
MODULEALIGN equ 1<<0                    ; align loaded modules on page
                                        ; boundaries
MEMINFO     equ 1<<1                    ; provide memory map
MAGIC       equ 0x1BADB002              ; magic number for bootloader to
                                        ; find the header
FLAGS       equ MODULEALIGN | MEMINFO   ; the multiboot flag field
CHECKSUM    equ -(MAGIC + FLAGS)        ; checksum required

; we set Page write-through, Writable, Present
; The PDE and PTE structure is very similar: 32 bits (4 bytes),
; where the highest 20 bits points to a PTE or PF, and the
; lowest 12 bits control access rights and other configurations.
; for details, pls refer to https://wiki.osdev.org/Paging
KERNEL_PT_CFG       equ 00000000000000000000000000001011b
KERNEL_PDT_ID_MAP   equ 00000000000000000000000010001011b

; the page directory used to boot the kernel into the higher half
section .data
align 4096
kernel_pt:
    times 1024 dd 0
kernel_pdt:
    dd KERNEL_PDT_ID_MAP
    times 1023 dd 0

section .bss
align 4
kernel_stack:
    resb KERNEL_STACK_SIZE

section .text
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

; the entry point, called by GRUB
loader:

; set up kernel_pdt to point to kernel_pt (both physical addresses)
; index of pdt is just the highest 10bits of the virtual address
; each entry has 4 bytes, thus we use index * 4
set_up_kernel_pdt:
    mov ecx, (kernel_pdt - KERNEL_START_VADDR + KERNEL_PDT_IDX*4)
    mov edx, (kernel_pt - KERNEL_START_VADDR)
    or  edx, KERNEL_PT_CFG
    mov [ecx], edx

; mapping:
;   0xC0000000 ~ 0xC0100000 ~ (0xC000000+kernel_physical_end) --->
;   0x00000000 ~ 0x00100000 ~ kernel_physical_end
set_up_kernel_pt:
    mov eax, (kernel_pt - KERNEL_START_VADDR)
    mov ecx, KERNEL_PT_CFG
.loop_pt:
    mov [eax], ecx
    add eax, 4
    add ecx, PAGE_FRAME_SIZE
    cmp ecx, kernel_physical_end
    jle .loop_pt

; assembly code executing at around 0x00100000
; enable paging for both actual location of kernel
; and its higher-half virtual location
enable_paging:
    mov ecx, (kernel_pdt - KERNEL_START_VADDR)
    mov cr3, ecx            ; load pdt

    mov ecx, cr4
    or  ecx, 0x00000010     ; enable PSE (4 MiB pages)
    mov cr4, ecx

    mov	ecx, cr0
    or  ecx, 0x80000001	    ; set the paging (PG) and protection (PE) bits
    mov cr0, ecx

    ; an jump can be done to a label to make
    ; eip point to a virtual address in the higher half
    ; assembly code will execute there
    lea ecx, [higher_half]
    jmp ecx

higher_half:
    ; code here executes in the higher half kernel
    ; eip is larger than 0xC0000000
    ; can continue kernel initialisation, calling C code, etc.

remove_identity_mapping:
    ; The entry mapping of the first 4 MB of virtual memory
    ; to the first 4 MB of physical memory can be removed now
    mov [kernel_pdt], DWORD 0
    invlpg [0]

    ; point esp to the start of the stack (end of memory area)
    mov esp, kernel_stack+KERNEL_STACK_SIZE

atest:                       ; only a test in the first step
    mov eax, 0xCAFEBABE      ; place the number 0xCAFEBABE in the register eax

enter_kmain:
    xchg bx, bx                 ; used for bochs breakpoint
    call kmain

.loop:
    jmp .loop                   ; loop forever
