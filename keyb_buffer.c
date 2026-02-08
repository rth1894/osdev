#include "keyb_buffer.h"

#define SIZE 64
#define MASK (SIZE - 1)

static volatile uint8_t buf[SIZE];
static volatile uint8_t head = 0, tail = 0;

void keyb_buffer_init(void) {
    head = 0;
    tail = 0;
}

bool keyb_has_data(void) {
    return head != tail;
}

uint8_t keyb_get_scancode(void) {
    if (head == tail) return 0;
    uint8_t sc = buf[tail];
    tail = (tail + 1) & MASK;
    return sc;
}

void keyb_buffer_push(uint8_t sc) {
    uint8_t next = (head + 1) & MASK;
    if (next == tail) return;
    buf[head] = sc;
    head = next;
}
