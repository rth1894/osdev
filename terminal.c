#include "vga.h"
#include "shell.h"
#include "terminal.h"

#define CMD_MAX 128

static char cmd_buffer[CMD_MAX];
static int  cmd_length = 0;

/* Pending action set by the last Enter keypress */
static int pending_action = SHELL_OK;

int terminal_get_action(void) {
    int a = pending_action;
    pending_action = SHELL_OK;   /* consume it */
    return a;
}

void terminal_handle_char(char c) {
    if (c == '\n') {
        vga_putc('\n');
        cmd_buffer[cmd_length] = '\0';
        pending_action = shell_execute(cmd_buffer);
        cmd_length = 0;
        if (pending_action == SHELL_OK)
            vga_print("> ");
        return;
    }

    if (c == '\b') {
        if (cmd_length > 0) {
            cmd_length--;
            vga_bksp();
        }
        return;
    }

    if (cmd_length < CMD_MAX - 1) {
        cmd_buffer[cmd_length++] = c;
        vga_putc(c);
    }
}
