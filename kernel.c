#include "idt.h"
#include "keyb.h"
#include "keyb_buffer.h"
#include "pic.h"
#include "vga.h"
#include "shell.h"
#include "terminal.h"
#include "tetris.h"

extern void dummy_isr(void);

#define MODE_SHELL  0
#define MODE_TETRIS 1

static void shell_welcome(void) {
    vga_clear();
    vga_print(
        "         __  __    ____  _____\n"
        "   _____/ /_/ /_  / __ \\/ ___/\n"
        "  / ___/ __/ __ \\/ / / /\\__ \\\n"
        " / /  / /_/ / / / /_/ /___/ /\n"
        "/_/   \\__/_/ /_/\\____//____/\n"
        "\n"
        "Welcome to rthOS.  Type 'help' for commands.\n"
        "Type 'play' to launch Tetris.\n"
        "\n"
        "> "
    );
}

void kernel_main(void) {
    idt_init();

    for (int i = 0; i < 256; i++)
        idt_set_gate(i, (uint32_t)dummy_isr);

    pic_remap();
    keyb_buffer_init();

    // boot into shell
    int mode = MODE_SHELL;
    shell_welcome();

    while (1) {
        keyb_poll();
        char c = keyb_get_char();

        if (mode == MODE_SHELL) {
            if (c) terminal_handle_char(c);

            int action = terminal_get_action();
            if (action == SHELL_PLAY) {
                mode = MODE_TETRIS;
                tetris_init();
            }
        }
        // tetris mode
        else {
            if (c) tetris_input(c);

            tetris_tick();
            tetris_render();

            if (tetris_wants_exit()) {
                mode = MODE_SHELL;
                shell_welcome();
            }
        }
    }
}
