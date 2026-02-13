#include "vga.h"
#include "io.h"

int strcmp(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return 1;
        a++;
        b++;
    }
    return (*a == *b) ? 0 : 1;
}

void shell_execute(const char* cmd) {
    if (strcmp(cmd, "help") == 0)
        vga_print("Commands: help, clear, echo, shutdown\n");

    else if (strcmp(cmd, "clear") == 0) {
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
    else if (strcmp(cmd, "echo") == 0) {
        vga_putc('\n');
    }
    else if (cmd[0] == 'e' && cmd[1] == 'c' && cmd[2] == 'h' && cmd[3] == 'o' && cmd[4] == ' ') {
        vga_print(cmd + 5);
        vga_putc('\n');
    }
    else if (strcmp(cmd, "shutdown") == 0) {
        outw(0x604, 0x2000); // thank you, qemu
    }
    else if (strcmp(cmd, "") == 0) {}
    else
        vga_print("Invalid command!\n");
}
