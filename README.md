# Hobby Operating System to run PONG(C, x86 Assembly)

Explores low-level systems programming, from bootloader design to kernel graphics and keyboard I/O.

---

## Features
- Custom bootloader and kernel written from scratch  
- Text-mode VGA graphics and minimal terminal interface  
- Keyboard driver using polling
- Experimental command parser and input buffer  
- Bootable ISO image for QEMU or real hardware testing  

---

## Development Setup

### 1. Building a Cross-Compiler
Following the [OSDev Wiki: GCC Cross-Compiler](https://wiki.osdev.org/GCC_Cross-Compiler)

**Commands used:**
```bash
wget -c https://ftp.gnu.org/gnu/gcc/gcc-12.1.0/gcc-12.1.0.tar.xz
wget -c https://ftp.gnu.org/gnu/binutils/binutils-2.42.tar.xz
# Alternative mirror
wget -c https://mirror.ibcp.fr/pub/gnu/binutils/binutils-2.42.tar.xz
```

---

---

## Keyboard Driver (PS/2)
- Basic keyboard input via polling of PS/2 controller
- Will transition to IRQ-based handling soon

**Resources:**
- [Interrupts – OSDev Wiki](https://wiki.osdev.org/Interrupts)
- [PS/2 Keyboard – OSDev Wiki](https://wiki.osdev.org/PS/2_Keyboard)
- [The Little Book about OS Development](https://littleosbook.github.io/)
- [Creating a Shell](https://wiki.osdev.org/Creating_A_Shell)

---

## Future Plans
- Add basic shell commands (e.g., `help`, `clear`, `pong`)
- Integrate a minimal file system layer
- Expand the Pong game and terminal UI

---

## Resources & References
- [OSDev Wiki](https://wiki.osdev.org)
- [Little OS Book](https://littleosbook.github.io/)
- [JamesM's Kernel Development Tutorials](https://www.jamesmolloy.co.uk/tutorial_html/)
