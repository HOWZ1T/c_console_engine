#ifndef CONSOLE_RECT_H
#define CONSOLE_RECT_H

#include <stdbool.h>

typedef struct Rect {
    int x, y, x1, y1, w, h;
} Rect;

Rect create_rect(int x, int y, int w, int h) {
    Rect r = {x, y, x+w, y+h, w, h};
    return r;
}

bool rect_overlap(Rect a, Rect b) {
    return !((a.x1 <= b.x || a.x >= b.x1) || (a.y1 <= b.y || a.y >= b.y1));
}

bool rect_in_bounds(Rect a, Rect b) {
    return a.x >= b.x && a.x <= b.x1 && a.y >= b.y && a.y <= b.y1;
}

#endif //CONSOLE_RECT_H
