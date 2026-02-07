#include "keyb_buffer.h"

#define SIZE 64
#define MASK (SIZE - 1) //63 -> 0b00111111

static volatile uint8_t buf[SIZE];
static volatile uint8_t head = 0, tail = 0;

void keyb_buffer_init(void) {
    head = 0;
    tail = 0;
}

bool keyb_has_data(void) {
    return head != tail;
}

/*
 * x86 instruvtions:
 * cli -> clear interrupt flag -> disable hw interrupts
 * sti -> set interrupt flag   -> reenable interrupt
*/

uint8_t keyb_get_scancode(void) {
    uint8_t sc;
    asm volatile("cli");
    if (head == tail) {
        asm volatile("sti");
        return 0;
    }

    sc = buf[tail];
    tail = (tail + 1) & MASK;
    asm volatile("sti");
    return sc;
}

void keyb_buffer_push(uint8_t sc) {
    uint8_t next = (head + 1) & MASK;
    if (next == tail)
        tail = (tail + 1) & MASK;
    buf[head] = sc;
    head = next;
}
