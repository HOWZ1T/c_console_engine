#include "include/console/console.h"
#include <stdio.h>

Panel panel, panel1, panel2;

void create() {
    panel = create_panel(5, 5, 10, 10, L"Panel");
    panel1 = create_panel(16, 16, 10, 10, L"Panel1");
    panel2 = create_panel(27, 27, 10, 10, L"Panel2");
    panel.component.attributes = FG_WHITE | BG_BLUE;
    panel.component.decorations = SimpleBorder;
    panel.component.alignments = TitleTopLeft;

    panel1.component.attributes = FG_WHITE | BG_BLUE;
    panel1.component.decorations = SingleBorder;
    panel1.component.alignments = TitleTopCenter;

    panel2.component.attributes = FG_WHITE | BG_BLUE;
    panel2.component.decorations = DoubleBorder;
    panel2.component.alignments = TitleTopRight;
}

void close() {}

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

void update(float elapsed) {
    wchar_t s[256];
    swprintf_s(s, 256, L"%s - FPS: %3.2f", L"Crack", (1.0f/elapsed));
    set_console_title(s);

    if (console_get_key(VK_ESCAPE).released) {
        stop_console();
    }
}

void render() {
    draw_numbers();
    panel.render(&panel);
    panel1.render(&panel1);
    panel2.render(&panel2);
}

int main(int argc, const char* argv[]) {
    create_console(80, 45, 16, 16, close, create, update, render);
    run_console();
    free_console();

    return 0;
}
