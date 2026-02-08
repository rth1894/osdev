global idt_load

idt_load:
    lidt [esp + 4]
    ret
