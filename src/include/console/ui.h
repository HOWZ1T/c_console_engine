#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include <limits.h>

enum Decorations {
    SingleBorder = 0x00000000,
    DoubleBorder = 0x00000001,
    SimpleBorder = 0x00000010,
};

enum Alignments {
    TitleTopLeft      = 0x00000001,
    TitleTopCenter    = 0x00000010,
    TitleTopRight     = 0x00000100,
    TitleBottomLeft   = 0x00001000,
    TitleBottomCenter = 0x00010000,
    TitleBottomRight  = 0x00100000,
};

typedef struct Component {
    int x, y, w, h;
    wchar_t* title;
    int decorations;
    int alignments;
    int attributes;

    void (*render)(struct Component *component);
} Component;

typedef struct Panel {
    Component component;
    void (*render)(struct Panel *panel);
} Panel;

#endif //CONSOLE_UI_H
