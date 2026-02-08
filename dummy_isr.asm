global dummy_isr

dummy_isr:
    cli
    pusha
    
    ; send EOI to master PIC
    mov al, 0x20
    out 0x20, al
    
    ; send EOI to slave PIC
    mov al, 0x20
    out 0xA0, al
    
    popa
    iret
