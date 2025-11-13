ISO = myos.iso
KERNEL = kernel.elf

all: $(ISO)

boot.o: boot.s
	i686-elf-as boot.s -o boot.o

kernel.o: kernel.c
	i686-elf-gcc -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

$(KERNEL): boot.o kernel.o
	i686-elf-gcc -T linker.ld -o $(KERNEL) -ffreestanding -O2 -nostdlib boot.o kernel.o -lgcc

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
