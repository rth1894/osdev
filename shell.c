#include "vga.h"
#include "io.h"
#include "shell.h"

static int strcmp_s(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return 1;
        a++; b++;
    }
    return (*a == *b) ? 0 : 1;
}

/*
 *   SHELL_OK      (0)  – normal command executed
 *   SHELL_PLAY    (1)  – user typed "play", switch to tetris
*/
int shell_execute(const char* cmd) {
    if (strcmp_s(cmd, "help") == 0) {
        vga_print("Commands: help, clear, echo, play, shutdown\n");
        vga_print("  help      - launch this menu\n");
        vga_print("  clear     - clear the screen\n");
        vga_print("  echo      - put something on the terminal\n");
        vga_print("  play      - launch ASCII Tetris\n");
        vga_print("  shutdown  - power off\n");

    }
    else if (strcmp_s(cmd, "clear") == 0) {
        vga_clear();
        vga_print(
            "         __  __    ____  _____\n"
            "   _____/ /_/ /_  / __ \\/ ___/\n"
            "  / ___/ __/ __ \\/ / / /\\__ \\\n"
            " / /  / /_/ / / / /_/ /___/ /\n"
            "/_/   \\__/_/ /_/\\____//____/\n"
        );
        vga_putc('\n');

    }
    else if (strcmp_s(cmd, "echo") == 0) vga_putc('\n');
    else if (cmd[0]=='e' && cmd[1]=='c' && cmd[2]=='h' && cmd[3]=='o' && cmd[4]==' ') {
        vga_print(cmd + 5);
        vga_putc('\n');
    }
    else if (strcmp_s(cmd, "play") == 0) return SHELL_PLAY;
    else if (strcmp_s(cmd, "shutdown") == 0) outw(0x604, 0x2000);

    else if (strcmp_s(cmd, "") == 0) {}
    else {
        vga_print("Unknown command. Type 'help' for a list.\n");
    }

    return SHELL_OK;
}
