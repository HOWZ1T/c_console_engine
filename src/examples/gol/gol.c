#include "console/console.h"

#define board_w 50
#define board_h 50
#define font_size 12
#define alive 1
#define dead 0

int prev_board[board_w][board_h];
int board[board_w][board_h];
float t_passed = 0, time_per_timestep = 100;

int rand_range(int min, int max) {
    return rand() % (max - min + 1) + min;
}

int get_live_neighbours(int x, int y) {
    int count = 0;
    for (int x1 = x-1; x1 <= x+1; x1++) {
        for (int y1 = y-1; y1 <= y+1; y1++) {
            if(x1 >= 0  && x1 < board_w && y1 >= 0 && y1 < board_h) {
                if (prev_board[x1][y1] == alive) {
                    count += 1;
                }
            }
        }
    }

    return count;
}

void tick() {
    // update board based on the gol rules
    for (int x = 0; x < board_w; x++) {
        for (int y = 0; y < board_h; y++) {
            int alive_neighbours = get_live_neighbours(x, y);
            int state = (prev_board[x][y] == 1) ? alive : dead;
            if (state == alive && (alive_neighbours == 2 || alive_neighbours == 3)) {
                state = alive;
            } else if (state == dead && alive_neighbours == 3) {
                state = alive;
            } else {
                state = dead;
            }

            board[x][y] = state;
        }
    }

    // update prev board state to current board state
    for (int x = 0; x < board_w; x++) {
        for (int y = 0; y < board_h; y++) {
            prev_board[x][y] = board[x][y];
        }
    }
}

void create() {
    for (int x = 0; x < board_w; x++) {
        for (int y = 0; y < board_h; y++) {
            int b = rand_range(0, 2);
            prev_board[x][y] = b;
            board[x][y] = b;
        }
    }
}

void close() {}

void update(float elapsed) {
    wchar_t s[256];
    swprintf_s(s, 256, L"%s - FPS: %3.2f", L"Game Of Life", (1.0f/elapsed));
    set_console_title(s);

    if (console_get_key(VK_ESCAPE).released) {
        stop_console();
    }

    while (t_passed >= time_per_timestep) {
        tick();
        t_passed -= time_per_timestep;
    }

    t_passed += elapsed * 1000;
}

void render() {
    console_clear(U_NULL, BG_BLACK);

    for (int x = 0; x < board_w; x++) {
        for (int y = 0; y < board_h; y++) {
            if (board[x][y] == 1) {
                console_draw(x, y, U_FULL_BLOCK, FG_GREEN);
            }
        }
    }
}

int main(int argc, const char* argv[]) {
    srand((unsigned) time(NULL));
    create_console(board_w, board_h, font_size, font_size, close, create, update, render);
    run_console();
    free_console();
    return 0;
}