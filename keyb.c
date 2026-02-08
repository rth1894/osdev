#include "keyb.h"
#include "keyb_buffer.h"
#include <stdint.h>

#define KEYMAP_SIZE 0x59

static int shift = 0;

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void keyb_poll(void) {
    if ((inb(0x64) & 1) == 0) return;
    uint8_t sc = inb(0x60);

    if (sc == 0xE0 || sc == 0xE1) return;
    if (sc >= 0x80) {
        if (sc != 0xAA && sc != 0xB6) return;
    } else {
        if (sc >= 0x59) return;
    }
    keyb_buffer_push(sc);
}

static const char keymap[KEYMAP_SIZE] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',0,
    '*',0,' '
};

static const char keymap_shift[KEYMAP_SIZE] = {
    0, 27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0,'A','S','D','F','G','H','J','K','L',':','"','~',
    0,'|','Z','X','C','V','B','N','M','<','>','?',0,
    '*',0,' '
};

char keyb_get_char(void) {
    if (!keyb_has_data())
        return 0;

    uint8_t sc = keyb_get_scancode();

    if (sc == 0x2A || sc == 0x36) {
        shift = 1;
        return 0;
    }
    if (sc == 0xAA || sc == 0xB6) {
        shift = 0;
        return 0;
    }
    if (sc & 0x80)
        return 0;
    if (sc >= KEYMAP_SIZE)
        return 0;

    return shift ? keymap_shift[sc] : keymap[sc];
}
