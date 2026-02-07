ISO = myos.iso
KERNEL = kernel.elf
OBJS = boot.o kernel.o vga.o keyb.o keyb_buffer.o keyb_isr.o

all: $(ISO)

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

isodir/boot/myiso.bin: $(KERNEL)
	mkdir -p isodir/boot
	cp $(KERNEL) isodir/boot/myiso.bin

isodir/boot/grub/grub.cfg: grub.cfg
	mkdir -p isodir/boot/grub
	cp grub.cfg isodir/boot/grub/grub.cfg

$(ISO): isodir/boot/myiso.bin isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) isodir

run: $(ISO)
	qemu-system-i386 -cdrom $(ISO)

clean:
	rm -rf *.o *.elf *.iso isodir
