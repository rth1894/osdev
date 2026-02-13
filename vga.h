#pragma once
#include <stdint.h>

static void newline();
static void vga_scroll(void);

void vga_bksp(void);
void vga_clear(void);
void vga_print(const char* str);
void vga_putc(char c);
void vga_update_cursor(void);
