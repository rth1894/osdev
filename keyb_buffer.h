#pragma once
#include <stdint.h>
#include <stdbool.h>

void keyb_buffer_init(void);
bool keyb_has_data(void);
uint8_t keyb_get_scancode(void);
void keyb_buffer_push(uint8_t sc);
