#include "console/console.h"
#include <stdio.h>

void create() {}

void close() {}

void update(float elapsed) {
    wchar_t s[256];
    swprintf_s(s, 256, L"%s - FPS: %3.2f", console_title(), (1.0f/elapsed));
    set_console_title((wchar_t *) s); // TODO: BUG SET CONSOLE TITLE BUFFER IN CONSOLE STRUCT TOO SMALL
}

void draw_numbers() {
    console_clear(U_NULL, BG_BLACK);

    wchar_t buff[256];
    bool flip = false;
    short col;
    for(int x = 0; x < console_width(); x++) {
        if (x%10 == 0) flip = !flip;
        if (flip) {
            col = FG_RED;
        } else {
            col = FG_GREEN;
        }
        swprintf_s(buff, 256, L"%d", x%10);
        console_draw(x, 0, buff[0], col | BG_BLACK);
    }

    for(int y = 1; y < console_height(); y++) {
        if (y%10 == 0) flip = !flip;
        if (flip) {
            col = FG_RED;
        } else {
            col = FG_GREEN;
        }
        swprintf_s(buff, 256, L"%d", y%10);
        console_draw(0, y, buff[0], col | BG_BLACK);
    }
}

void render() {
    draw_numbers();
}

int main(int argc, const char* argv[]) {
    create_console(L"Crack", 80, 45, 16, 16, close, create, update, render);
    run_console();
    free_console();
    return 0;
}
