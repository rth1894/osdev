#include "keyb.h"
#include "keyb_buffer.h"

static int shift = 0;

static const char keymap[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',0,
    '*',0,' ',0
};

static const char keymap_shift[128] = {
    0, 27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0,'A','S','D','F','G','H','J','K','L',':','"','~',
    0,'\\','Z','X','C','V','B','N','M','<','>','?',0,
    '*',0,' ',0
};

//0x80 -> 10000000
char keyb_get_char(void) {
    if (!keyb_has_data()) return 0;

    uint8_t sc = keyb_get_scancode();
    if (sc == 0x2A || sc == 0x36) shift = 1;
    else if (sc == 0xAA || sc == 0xB6) shift = 0;
    else if (sc & 0x80) return 0;

    return shift ? keymap_shift[sc] : keymap[sc];

    return 0;
}
