global set_pd
global invalidate_pte

section .text:

set_pd:
    mov eax, [esp+4]
    and eax, 0xFFFFF000
    mov	cr3, eax

    mov	ecx, cr0
    or  ecx, 0x80000001	    ; set the paging (PG) and protection (PE) bits
    mov cr0, ecx

    ret

invalidate_pte:
    mov eax, [esp+4]    ; loads the virtual address which page table entry
                        ; will be flushed
    invlpg [eax]
    ret