/*
 * tetris.c  –  ASCII Tetris for the bare-metal VGA kernel
 *
 * Controls (polling-based, no interrupts needed):
 *   a / left   – move left
 *   d / right  – move right
 *   s / down   – soft drop
 *   w / up     – rotate CW
 *   space      – hard drop
 *   q          – restart after game-over OR quit when game not over
 *   escape     - quit the game
*/

#include "vga.h"
#include "tetris.h"

#define NUM_PIECES  7
#define NUM_ROTS    4

static void mem_set(uint8_t *p, uint8_t v, int n) {
    for (int i = 0; i < n; i++) p[i] = v;
}

static int int_to_str(int v, char *buf) {
    if (v == 0) { buf[0] = '0'; buf[1] = '\0'; return 1; }
    char tmp[12]; int i = 0;
    while (v > 0) { tmp[i++] = '0' + (v % 10); v /= 10; }
    int len = i;
    for (int j = 0; j < len; j++) buf[j] = tmp[len - 1 - j];
    buf[len] = '\0';
    return len;
}


/* Each piece: [rotation][cell][x or y]  – offsets within 4×4 box */
static const int8_t PIECES[NUM_PIECES][NUM_ROTS][4][2] = {
    // I
    {
        {{0,1},{1,1},{2,1},{3,1}},
        {{2,0},{2,1},{2,2},{2,3}},
        {{0,2},{1,2},{2,2},{3,2}},
        {{1,0},{1,1},{1,2},{1,3}},
    },
    // O
    {
        {{1,0},{2,0},{1,1},{2,1}},
        {{1,0},{2,0},{1,1},{2,1}},
        {{1,0},{2,0},{1,1},{2,1}},
        {{1,0},{2,0},{1,1},{2,1}},
    },
    // T
    {
        {{1,0},{0,1},{1,1},{2,1}},
        {{1,0},{1,1},{2,1},{1,2}},
        {{0,1},{1,1},{2,1},{1,2}},
        {{1,0},{0,1},{1,1},{1,2}},
    },
    // S
    {
        {{1,0},{2,0},{0,1},{1,1}},
        {{1,0},{1,1},{2,1},{2,2}},
        {{1,1},{2,1},{0,2},{1,2}},
        {{0,0},{0,1},{1,1},{1,2}},
    },
    // Z
    {
        {{0,0},{1,0},{1,1},{2,1}},
        {{2,0},{1,1},{2,1},{1,2}},
        {{0,1},{1,1},{1,2},{2,2}},
        {{1,0},{0,1},{1,1},{0,2}},
    },
    // J
    {
        {{0,0},{0,1},{1,1},{2,1}},
        {{1,0},{2,0},{1,1},{1,2}},
        {{0,1},{1,1},{2,1},{2,2}},
        {{1,0},{1,1},{0,2},{1,2}},
    },
    // L
    {
        {{2,0},{0,1},{1,1},{2,1}},
        {{1,0},{1,1},{1,2},{2,2}},
        {{0,1},{1,1},{2,1},{0,2}},
        {{0,0},{1,0},{1,1},{1,2}},
    },
};

static const uint8_t PIECE_COLOR[NUM_PIECES] = {
    11,  // I – light cyan
    14,  // O – yellow
    13,  // T – magenta
    10,  // S – light green
    12,  // Z – light red
     9,  // J – light blue
     6,  // L – brown/orange
};

// Piece glyphs
static const char PIECE_CHAR[NUM_PIECES] = { '#', '@', '%', '&', '*', '+', '=' };

static uint8_t board[BOARD_H][BOARD_W];   // 0: empty, else piece char

static int cur_type;
static int cur_rot;
static int cur_x, cur_y;

static int next_type;

static int score;
static int lines_cleared;
static int level;
static int game_state;

static uint32_t rng_state = 12345;

static int rng_next(void) {
    rng_state = rng_state * 1664525u + 1013904223u;
    return (int)((rng_state >> 16) & 0x7FFF);
}

static int rand_piece(void) {
    return rng_next() % NUM_PIECES;
}


// we call tetris_tick() from the main loop.  We use a simple counter to slow gravity down instead of a real timer
#define TICKS_PER_DROP_BASE  60000
static volatile int tick_counter = 0;

static int board_get(int bx, int by) {
    if (bx < 0 || bx >= BOARD_W || by < 0 || by >= BOARD_H) return 1;
    return board[by][bx] ? 1 : 0;
}

static int collides(int type, int rot, int px, int py) {
    for (int i = 0; i < 4; i++) {
        int cx = px + PIECES[type][rot][i][0];
        int cy = py + PIECES[type][rot][i][1];
        if (board_get(cx, cy)) return 1;
    }
    return 0;
}

static void lock_piece(void) {
    char ch = PIECE_CHAR[cur_type];
    for (int i = 0; i < 4; i++) {
        int cx = cur_x + PIECES[cur_type][cur_rot][i][0];
        int cy = cur_y + PIECES[cur_type][cur_rot][i][1];
        if (cy >= 0 && cy < BOARD_H && cx >= 0 && cx < BOARD_W) board[cy][cx] = (uint8_t)ch;
    }
}

static int clear_lines(void) {
    int cleared = 0;
    for (int r = BOARD_H - 1; r >= 0; ) {
        int full = 1;
        for (int c = 0; c < BOARD_W; c++)
            if (!board[r][c]) { full = 0; break; }

        if (full) {
            // shift everything above down
            for (int rr = r; rr > 0; rr--)
                for (int c = 0; c < BOARD_W; c++)
                    board[rr][c] = board[rr-1][c];
            for (int c = 0; c < BOARD_W; c++)
                board[0][c] = 0;
            cleared++;
        }
        else r--;
    }
    return cleared;
}

// Spawn a new piece at the top
static void spawn_piece(void) {
    cur_type = next_type;
    next_type = rand_piece();
    cur_rot = 0;
    cur_x = BOARD_W / 2 - 2;
    cur_y = 0;

    if (collides(cur_type, cur_rot, cur_x, cur_y)) game_state = TETRIS_GAMEOVER;
}

static const int LINE_SCORES[5] = { 0, 100, 300, 500, 800 };

static void add_score(int n) {
    score += LINE_SCORES[n] * (level + 1);
    lines_cleared += n;
    level = lines_cleared / 10;
    if (level > 9) level = 9;
}

/*
 * Board box occupies columns BOARD_X..(BOARD_X+BOARD_W+1)
 * rows BOARD_Y..(BOARD_Y+BOARD_H+1).
 * Each board cell is 1 character wide.
*/

#define SCR_LEFT   (BOARD_X)
#define SCR_TOP    (BOARD_Y)
#define INFO_X     (BOARD_X + BOARD_W + 4)
#define INFO_Y     (BOARD_Y)

static void draw_str_at(int x, int y, const char *s) {
    while (*s) { vga_put_at(x++, y, *s++); }
}

static void draw_int_at(int x, int y, int v, int pad) {
    char buf[12];
    int len = int_to_str(v, buf);
    int i = 0;
    while (buf[i]) { vga_put_at(x + i, y, buf[i]); i++; }
    while (i < pad) { vga_put_at(x + i, y, ' '); i++; }
}

static void draw_border(void) {
    vga_put_at(SCR_LEFT, SCR_TOP, '+');
    for (int c = 0; c < BOARD_W; c++)
        vga_put_at(SCR_LEFT + 1 + c, SCR_TOP, '-');
    vga_put_at(SCR_LEFT + BOARD_W + 1, SCR_TOP, '+');

    for (int r = 0; r < BOARD_H; r++) {
        vga_put_at(SCR_LEFT,             SCR_TOP + 1 + r, '|');
        vga_put_at(SCR_LEFT + BOARD_W + 1, SCR_TOP + 1 + r, '|');
    }

    vga_put_at(SCR_LEFT, SCR_TOP + BOARD_H + 1, '+');
    for (int c = 0; c < BOARD_W; c++)
        vga_put_at(SCR_LEFT + 1 + c, SCR_TOP + BOARD_H + 1, '-');
    vga_put_at(SCR_LEFT + BOARD_W + 1, SCR_TOP + BOARD_H + 1, '+');
}

static void draw_board_cells(void) {
    for (int r = 0; r < BOARD_H; r++) {
        for (int c = 0; c < BOARD_W; c++) {
            if (board[r][c])
                vga_put_at(SCR_LEFT + 1 + c, SCR_TOP + 1 + r, (char)board[r][c]);
            else
                vga_put_at(SCR_LEFT + 1 + c, SCR_TOP + 1 + r, ' ');
        }
    }
}

static void draw_current(char ch) {
    (void)ch; // type-specific char
    char pc = PIECE_CHAR[cur_type];
    for (int i = 0; i < 4; i++) {
        int cx = cur_x + PIECES[cur_type][cur_rot][i][0];
        int cy = cur_y + PIECES[cur_type][cur_rot][i][1];
        if (cy >= 0 && cy < BOARD_H && cx >= 0 && cx < BOARD_W)
            vga_put_color_at(SCR_LEFT + 1 + cx, SCR_TOP + 1 + cy, pc, PIECE_COLOR[cur_type], 0);
    }
}

// ghost piece
static void draw_ghost(char ch) {
    (void)ch;
    int gy = cur_y;
    while (!collides(cur_type, cur_rot, cur_x, gy + 1)) gy++;
    if (gy == cur_y) return;

    for (int i = 0; i < 4; i++) {
        int cx = cur_x + PIECES[cur_type][cur_rot][i][0];
        int cy = gy   + PIECES[cur_type][cur_rot][i][1];
        if (cy >= 0 && cy < BOARD_H && cx >= 0 && cx < BOARD_W) {
            int occupied = 0;
            for (int j = 0; j < 4; j++)
                if (cur_x + PIECES[cur_type][cur_rot][j][0] == cx &&
                    cur_y + PIECES[cur_type][cur_rot][j][1] == cy)
                    occupied = 1;
            if (!occupied && !board[cy][cx])
                vga_put_color_at(SCR_LEFT + 1 + cx, SCR_TOP + 1 + cy, '.', 8, 0);
        }
    }
}

// info panel
static void draw_info(void) {
    draw_str_at(INFO_X, INFO_Y,     "TETRIS");
    draw_str_at(INFO_X, INFO_Y + 2, "Score:");
    draw_int_at(INFO_X, INFO_Y + 3, score, 8);
    draw_str_at(INFO_X, INFO_Y + 5, "Lines:");
    draw_int_at(INFO_X, INFO_Y + 6, lines_cleared, 8);
    draw_str_at(INFO_X, INFO_Y + 8, "Level:");
    draw_int_at(INFO_X, INFO_Y + 9, level + 1, 8);

    draw_str_at(INFO_X, INFO_Y + 11, "Next:");
    for (int r = 0; r < 4; r++)
        draw_str_at(INFO_X, INFO_Y + 13 + r, "    ");
    for (int i = 0; i < 4; i++) {
        int nx = PIECES[next_type][0][i][0];
        int ny = PIECES[next_type][0][i][1];
        vga_put_at(INFO_X + nx, INFO_Y + 13 + ny, PIECE_CHAR[next_type]);
    }

    draw_str_at(INFO_X, INFO_Y + 15, "Controls:");
    draw_str_at(INFO_X, INFO_Y + 16, "a/d  move");
    draw_str_at(INFO_X, INFO_Y + 17, "s    drop");
    draw_str_at(INFO_X, INFO_Y + 18, "w    rotate");
    draw_str_at(INFO_X, INFO_Y + 19, "SPC  hard drop");
    draw_str_at(INFO_X, INFO_Y + 20, "q    restart");
    draw_str_at(INFO_X, INFO_Y + 21, "ESC  quit");
}

static void draw_game_over(void) {
    draw_str_at(SCR_LEFT + 1, SCR_TOP + 8,  "          ");
    draw_str_at(SCR_LEFT + 1, SCR_TOP + 9,  " GAME  OV ");
    draw_str_at(SCR_LEFT + 1, SCR_TOP + 10, "  -ER-    ");
    draw_str_at(SCR_LEFT + 1, SCR_TOP + 11, "          ");
    draw_str_at(SCR_LEFT + 1, SCR_TOP + 12, " Press q  ");
    draw_str_at(SCR_LEFT + 1, SCR_TOP + 13, " to retry ");
    draw_str_at(SCR_LEFT + 1, SCR_TOP + 14, "          ");
}

void tetris_init(void) {
    mem_set((uint8_t*)board, 0, sizeof(board));
    score        = 0;
    lines_cleared = 0;
    level        = 0;
    game_state   = TETRIS_PLAYING;
    tick_counter = 0;
    rng_state   += 90210;   // to make each game feel different 

    next_type = rand_piece();
    spawn_piece();

    vga_clear();
    draw_border();
    draw_info();
}

void tetris_draw_board(void) {
    draw_border();
}

void tetris_draw_piece(int px, int py) {
    (void)px; (void)py;
}

void tetris_render(void) {
    draw_board_cells();
    draw_ghost('.');
    draw_current(PIECE_CHAR[cur_type]);
    draw_info();

    if (game_state == TETRIS_GAMEOVER)
        draw_game_over();
}

// process one gravity tick.  returns 1 if game just ended.
int tetris_tick(void) {
    if (game_state != TETRIS_PLAYING) return (game_state == TETRIS_GAMEOVER);

    tick_counter++;
    // speed: higher = faster
    int threshold = TICKS_PER_DROP_BASE - level * 5500;
    if (threshold < 3000) threshold = 3000;

    if (tick_counter < threshold) return 0;
    tick_counter = 0;

    // try to move down
    if (!collides(cur_type, cur_rot, cur_x, cur_y + 1)) cur_y++;
    // land
    else {
        lock_piece();
        int n = clear_lines();
        add_score(n);
        spawn_piece();
        if (game_state == TETRIS_GAMEOVER) return 1;
    }
    return 0;
}

void tetris_input(char c) {
    if (c == 27) {
        game_state = TETRIS_EXIT;
        return;
    }

    if (game_state == TETRIS_GAMEOVER) {
        if (c == 'q' || c == 'Q') tetris_init();  // restart
        return;
    }

    if (game_state == TETRIS_EXIT) return;

    if (c == 'q' || c == 'Q') {
        game_state = TETRIS_EXIT;
        return;
    }

    // left
    if (c == 'a' || c == 'A') {
        if (!collides(cur_type, cur_rot, cur_x - 1, cur_y))
            cur_x--;
    }
    // right
    else if (c == 'd' || c == 'D') {
        if (!collides(cur_type, cur_rot, cur_x + 1, cur_y))
            cur_x++;

    }
    // soft drop
    else if (c == 's' || c == 'S') {
        if (!collides(cur_type, cur_rot, cur_x, cur_y + 1)) {
            cur_y++;
            score += 1;
        }

    }
    // rotate (clockwise)
    else if (c == 'w' || c == 'W') {
        int new_rot = (cur_rot + 1) % NUM_ROTS;
        if (!collides(cur_type, new_rot, cur_x, cur_y)) cur_rot = new_rot;
        else if (!collides(cur_type, new_rot, cur_x - 1, cur_y)) { cur_x--; cur_rot = new_rot; }
        else if (!collides(cur_type, new_rot, cur_x + 1, cur_y)) { cur_x++; cur_rot = new_rot; }

    }
    // hard drop
    else if (c == ' ') {
        while (!collides(cur_type, cur_rot, cur_x, cur_y + 1)) {
            cur_y++;
            score += 2;
        }
        tick_counter = TICKS_PER_DROP_BASE;
    }
}

int tetris_game_over(void) {
    return game_state == TETRIS_GAMEOVER;
}

int tetris_wants_exit(void) {
    return game_state == TETRIS_EXIT;
}
