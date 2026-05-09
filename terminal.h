#pragma once

void terminal_handle_char(char c);
int  terminal_get_action(void);   /* returns SHELL_* after Enter is pressed */
