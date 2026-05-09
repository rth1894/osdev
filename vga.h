#pragma once

#include <stdint.h>

void vga_clear(void);
void vga_putc(char c);
void vga_print(const char* str);

void vga_bksp(void);

void vga_update_cursor(void);
void vga_set_cursor(int x, int y);
void vga_put_at(int x, int y, char c);

void vga_set_color(uint8_t fg, uint8_t bg);
void vga_put_color_at(int x, int y, char c, uint8_t fg, uint8_t bg);
