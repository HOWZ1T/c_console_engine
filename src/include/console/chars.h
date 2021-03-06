#ifndef CONSOLE_CHARS_H
#define CONSOLE_CHARS_H

#ifndef UNICODE
#define UNICODE
#endif

enum UNICODE_CHARS {
    U_NULL = 0x0000,
    U_SPACE = 0x0020,

    U_FULL_BLOCK = 0x2588,
    U_LOWER_HALF_BLOCK = 0x2584,
    U_LOWER_QUARTER_BLOCK = 0x2582,
    U_LOWER_EIGHTH_BLOCK = 0x2581,

    U_UPPER_HALF_BLOCK = 0x2580,
    U_UPPER_EIGHTH_BLOCK = 0x2594,

    U_LIGHT_SHADE = 0x2591,
    U_MEDIUM_SHADE = 0x2592,
    U_DARK_SHADE = 0x2593,

    U_ARROW_LEFT = 0x2190,
    U_ARROW_UP = 0x2191,
    U_ARROW_RIGHT = 0x2192,
    U_ARROW_DOWN = 0x2193,

    U_SMILEY = 0x263A,
    U_SMILEY_FILLED = 0x263B,

    U_HEART = 0x2665,
    U_DIAMOND = 0x2666,
    U_CLUB = 0x2663,
    U_SPADES = 0x2660,

    U_LINE_VERT_DOUBLE = 0x2551,
    U_LINE_HORZ_DOUBLE = 0x2550,
    U_LINE_TOP_LEFT_DOUBLE = 0x2554,
    U_LINE_TOP_RIGHT_DOUBLE = 0x2557,
    U_LINE_BOTTOM_LEFT_DOUBLE = 0x255A,
    U_LINE_BOTTOM_RIGHT_DOUBLE = 0x255D,

    U_LINE_VERT_SINGLE = 0x2502,
    U_LINE_HORZ_SINGLE = 0x2500,
    U_LINE_TOP_LEFT_SINGLE = 0x250C,
    U_LINE_TOP_RIGHT_SINGLE = 0x2510,
    U_LINE_BOTTOM_LEFT_SINGLE = 0x2514,
    U_LINE_BOTTOM_RIGHT_SINGLE = 0x2518,

    U_LINE_RIGHT_CAP_SINGLE = 0x251C,
    U_LINE_LEFT_CAP_SINGLE = 0x2524,

    U_LINE_RIGHT_CAP_DOUBLE = 0x255E,
    U_LINE_LEFT_CAP_DOUBLE = 0x2561,
};

#endif //CONSOLE_CHARS_H
