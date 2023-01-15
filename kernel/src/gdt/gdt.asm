[bits 64]
load_gdt:
    lgdt [rdi]   ; will contain first parameter
    mov ax, 0x10 ; selector for kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    pop rdi       ; return address in rdi
    mov rax, 0x08 ; selector for kernel code segment
    push rax
    push rdi
    retfq
GLOBAL load_gdt