#include <stdint.h>

#include "vga.h"
#include "io.h"

#define VGA_ADDR   0xB8000
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

static uint16_t* const VGA_MEMORY = (uint16_t*) VGA_ADDR;

static uint8_t row = 0;
static uint8_t col = 0;

static uint8_t color = 0x0F;

static void vga_scroll(void) {
    for (int r = 1; r < VGA_HEIGHT; r++)
        for (int c = 0; c < VGA_WIDTH; c++)
            VGA_MEMORY[(r - 1) * VGA_WIDTH + c] = VGA_MEMORY[r * VGA_WIDTH + c];

    for (int c = 0; c < VGA_WIDTH; c++)
        VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + c] = (color << 8) | ' ';

    row = VGA_HEIGHT - 1;
    col = 0;
}

void vga_update_cursor(void) {
    uint16_t pos = row * VGA_WIDTH + col;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));

    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void vga_set_cursor(int x, int y) {
    col = x;
    row = y;
    vga_update_cursor();
}

void vga_clear(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        VGA_MEMORY[i] = (color << 8) | ' ';

    row = 0;
    col = 0;
    vga_update_cursor();
}

void vga_putc(char c) {
    if (c == '\n') {
        col = 0;
        row++;
    }
    else {
        VGA_MEMORY[row * VGA_WIDTH + col] = (color << 8) | (uint8_t)c;
        col++;

        if (col >= VGA_WIDTH) {
            col = 0;
            row++;
        }
    }

    if (row >= VGA_HEIGHT) vga_scroll();
    vga_update_cursor();
}

void vga_print(const char* str) {
    while (*str) vga_putc(*str++);
}

void vga_set_color(uint8_t fg, uint8_t bg) {
    color = (bg << 4) | (fg & 0x0F);
}

void vga_put_color_at(int x, int y, char c, uint8_t fg, uint8_t bg) {
    if (x < 0 || y < 0 || x >= VGA_WIDTH || y >= VGA_HEIGHT) return;
    uint8_t attr = (bg << 4) | (fg & 0x0F);
    VGA_MEMORY[y * VGA_WIDTH + x] = ((uint16_t)attr << 8) | (uint8_t)c;
}

void vga_bksp(void) {
    if (col == 0) return;
    col--;

    VGA_MEMORY[row * VGA_WIDTH + col] = (color << 8) | ' ';
    vga_update_cursor();
}

void vga_put_at(int x, int y, char c) {
    if (x < 0 || y < 0 || x >= VGA_WIDTH || y >= VGA_HEIGHT) return;
    VGA_MEMORY[y * VGA_WIDTH + x] = (color << 8) | (uint8_t)c;
}
