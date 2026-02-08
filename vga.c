#include "vga.h"

#define VGA_ADDR   0xB8000
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

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
        newline();
        return;
    }
    if (c == '\b') {
        if (col > 0)
            col--;
        else if (row > 0) {
            row--;
            col = VGA_WIDTH - 1;
        }
        vga[row * VGA_WIDTH + col] = (color << 8) | ' ';
        return;
    }

    vga[row * VGA_WIDTH + col] = (color << 8) | c;
    if (++col >= VGA_WIDTH)
        newline();
}

void vga_print(const char* str) {
    while(*str)
        vga_putc(*str++);

}
