#pragma once
#include <stdint.h>

// board dimensions
#define BOARD_W   10
#define BOARD_H   20

#define BOARD_X   30
#define BOARD_Y    2

// Game states
#define TETRIS_PLAYING  0
#define TETRIS_PAUSED   1
#define TETRIS_GAMEOVER 2
#define TETRIS_EXIT     3

void tetris_init(void);
void tetris_draw_board(void);
void tetris_draw_piece(int px, int py);
int  tetris_tick(void);
void tetris_input(char c);
void tetris_render(void);
int  tetris_game_over(void);
int  tetris_wants_exit(void);
