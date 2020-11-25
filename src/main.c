#include "console/console.h"
#include "adt/vector.h"
#include <stdio.h>

void create() {}

void close() {}

Point p1 = {40, 23}, p2 = {40, 23};

void update(float elapsed) {
    wchar_t s[256];
    swprintf_s(s, 256, L"%s - FPS: %3.2f", L"Crack", (1.0f/elapsed));
    set_console_title(s);

    if (console_get_key(VK_ESCAPE).released) {
        stop_console();
    }

    if (console_get_mouse(1).held) {
        p2.x = console_mouse_x();
        p2.y = console_mouse_y();
    }
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
    console_line(p1.x, p1.y, p2.x, p2.y, U_FULL_BLOCK, FG_CYAN);
    console_rect_fill(1, 1, 10, 10, U_FULL_BLOCK, FG_BLUE);
    console_ellipse_fill(10, 10, 40, 23, U_FULL_BLOCK, FG_DARK_MAGENTA);
}

int main(int argc, const char* argv[]) {
    create_console(80, 45, 16, 16, close, create, update, render);
    run_console();
    free_console();

    return 0;
}
