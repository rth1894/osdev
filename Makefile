ISO = myos.iso
KERNEL = kernel.elf
OBJS = boot.o kernel.o vga.o keyb.o keyb_buffer.o dummy_isr.o idt.o pic.o idt_load.o

all: $(ISO)

dummy_isr.o: dummy_isr.asm
	nasm -f elf32 dummy_isr.asm -o dummy_isr.o

idt.o: idt.c
	i686-elf-gcc -c idt.c -o idt.o -ffreestanding

pic.o: pic.c
	i686-elf-gcc -c pic.c -o pic.o -ffreestanding

keyboard.o: keyboard.c
	i686-elf-gcc -c keyboard.c -o keyboard.o -ffreestanding

idt_load.o: idt_load.asm
	nasm -f elf32 idt_load.asm -o idt_load.o

keyboard_isr.o: keyboard_isr.asm
	nasm -f elf32 keyboard_isr.asm -o keyboard_isr.o

vga.o: vga.c
	i686-elf-gcc -c vga.c -o vga.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

keyb.o: keyb.c
	i686-elf-gcc -c keyb.c -o keyb.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

keyb_buffer.o: keyb_buffer.c
	i686-elf-gcc -c keyb_buffer.c -o keyb_buffer.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

keyb_isr.o: keyb_isr.asm
	nasm -f elf32 keyb_isr.asm -o keyb_isr.o

boot.o: boot.s
	i686-elf-as boot.s -o boot.o

kernel.o: kernel.c
	i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

$(KERNEL): $(OBJS)
	i686-elf-gcc -T linker.ld -o $(KERNEL) -ffreestanding -O2 -nostdlib $(OBJS) -lgcc

isodir/boot/kernel.elf: $(KERNEL)
	mkdir -p isodir/boot
	cp $(KERNEL) isodir/boot/kernel.elf

isodir/boot/grub/grub.cfg: grub.cfg
	mkdir -p isodir/boot/grub
	cp grub.cfg isodir/boot/grub/grub.cfg

$(ISO): isodir/boot/kernel.elf isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) isodir

run: $(ISO)
	qemu-system-i386 -cdrom $(ISO)

clean:
	rm -rf *.o *.elf *.iso isodir
