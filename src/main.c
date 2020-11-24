#include "console/console.h"
#include "adt/vector.h"
#include <stdio.h>

void create() {}

void close() {}

void update(float elapsed) {
    wchar_t s[256];
    swprintf_s(s, 256, L"%s - FPS: %3.2f", L"Crack", (1.0f/elapsed));
    set_console_title(s);
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
    /*create_console(80, 45, 16, 16, close, create, update, render);
    run_console();
    free_console();*/

    char buff[255];
    vec v = vec_create(1);
    vec_dump(buff, &v);
    printf("%s\n", buff);

    for (int i = 0; i <= 255; i++) {
        vec_add(&v, (void *) i);
        vec_dump(buff, &v);
        printf("%2d | %s\n", (int)vec_at(&v, i), buff);
    }

    printf("\nmoving to remove\n\n");

    for (int i = 0; i <= 255; i++) {
        int x = (int) vec_remove(&v);
        vec_dump(buff, &v);
        printf("%2d | %s\n", x, buff);
    }

    vec_free(&v);
    return 0;
}
