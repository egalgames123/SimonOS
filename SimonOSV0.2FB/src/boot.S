section .multiboot_header
header_start:
    dd 0xe85250d6                ; Multiboot2 magic
    dd 0                         ; Architecture (i386)
    dd header_end - header_start ; Header length
    ; Checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

    ; Framebuffer tag
    align 8
    dw 5                         ; Type: framebuffer
    dw 1                         ; Flags: optional
    dd 20                        ; Size
    dd 1024                      ; Width
    dd 768                       ; Height
    dd 32                        ; Depth (bpp)

    ; End tag
    align 8
    dw 0                         ; Type: end
    dw 0                         ; Flags
    dd 8                         ; Size
header_end:

section .bss
align 16
stack_bottom:
    resb 16384                   ; 16KB stack
stack_top:

section .text
global _start
extern kernel_main

_start:
    mov esp, stack_top           ; Set up stack
    and esp, -16                ; Align stack

    push ebx                     ; Multiboot info struct
    push eax                     ; Magic number

    cld                          ; Clear direction flag
    call kernel_main

    cli
.hang: hlt
    jmp .hang