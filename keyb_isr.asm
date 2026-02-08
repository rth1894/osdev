BITS 32
SECTION .text
global keyb_isr
extern keyb_ring_buf
extern keyb_ring_head
extern keyb_ring_tail

KEYB_RING_MASK equ 63

keyb_isr:
    pusha

    in al, 0x60

    cmp al, 0xE0
    je .skip_push
    cmp al, 0xE1
    je .skip_push
    cmp al, 0x80
    jb .is_make
    cmp al, 0xAA
    je .push_sc
    cmp al, 0xB6
    je .push_sc
    jmp .skip_push

.is_make:
    cmp al, 0x59
    jae .skip_push

.push_sc:
    movzx ebx, al
    movzx ecx, byte [keyb_ring_head]
    mov edx, ecx
    add edx, 1
    and edx, KEYB_RING_MASK
    movzx eax, byte [keyb_ring_tail]
    cmp edx, eax
    je .skip_push
    mov eax, keyb_ring_buf
    add eax, ecx
    mov [eax], bl
    mov byte [keyb_ring_head], dl

.skip_push:
    mov al, 0x20
    out 0x20, al

    popa
    iret
