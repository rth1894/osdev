global keyb_isr
extern keyb_buffer_push ; trust me bro

section .text
keyb_isr:
    pusha

    in al, 0x64
    test al, 1
    jz .done

    in al, 0x60
    push eax
    call keyb_buffer_push
    add esp, 4

.done:
    mov al, 0x20
    out 0x20, al ; 0x20 -> EOI (and also PIC command port??)
    popa
    iret  ; ret crashed :(
