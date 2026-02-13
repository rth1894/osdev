#include <stdint.h>

#include "vga.h"
#include "io.h"

#define VGA_ADDR   0xB8000
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

static uint16_t* vga = (uint16_t*) VGA_ADDR;
static uint8_t row = 0, col =0;
static uint8_t color = 0x0F; // white fore, black back

static void newline() {
    col = 0;
    if (++row >= VGA_HEIGHT)
        row = VGA_HEIGHT - 1; 
}

void vga_clear(void) {
    for (int i=0; i<VGA_HEIGHT*VGA_WIDTH; i++)
        vga[i] = (color << 8) | ' ';
    row = 0;
    col = 0;
}

void vga_putc(char c) {
    if (c == '\n') {
        col = 0;
        row++;
    }
    else {
        VGA_MEMORY[row * VGA_WIDTH + col] = (color << 8) | c;
        col++;

        if (col >= VGA_WIDTH) {
            col = 0;
            row++;
        }
    }
    if (row >= VGA_HEIGHT) {
        vga_scroll();
        row = VGA_HEIGHT - 1;
    }

    vga_update_cursor();
}

void vga_print(const char* str) {
    while(*str)
        vga_putc(*str++);

}

static void vga_scroll(void) {
    for (int r=1; r<VGA_HEIGHT; r++) {
        for (int c=0; c<VGA_WIDTH; c++) {
            VGA_MEMORY[(r - 1) * VGA_WIDTH + c] = VGA_MEMORY[r * VGA_WIDTH + c];
        }
    }

    for (int c=0; c<VGA_WIDTH; c++)
        VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + c] = (color << 8) | ' ';

    row = VGA_HEIGHT - 1;
    col = 0;
}

void vga_update_cursor(void) {
    uint16_t pos = row * VGA_WIDTH + col;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));

    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)(pos >> 8) & 0xFF);
}

void vga_bksp(void) {
    if (col == 0) return;
    col--;

    VGA_MEMORY[row * VGA_WIDTH + col] = (color << 8) | ' ';
    vga_update_cursor();
}
