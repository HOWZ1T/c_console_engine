#ifndef CONSOLE_H
#define CONSOLE_H

#ifndef UNICODE
#define UNICODE
#endif


#include <windows.h>
#include <stdbool.h>
#include <stdlib.h>
#include <strsafe.h>
#include <time.h>
#include <math.h>

#include "colors.h"
#include "chars.h"
#include "../adt/vector.h"

/*   ---   STRUCTS   ---   */

typedef struct Console {
    HANDLE h_console, h_in;
    SMALL_RECT rect_window;
    CHAR_INFO* scr_buff;
    CONSOLE_CURSOR_INFO cursor_info;
    int width, height, font_w, font_h;
    short clear_color, clear_char;

    void (*on_close)();
    void (*on_create)();
    void (*update)(float elapsed_time);
    void (*render)();
} Console;

typedef struct Point {
    int x, y;
} Point;

/*   ---   MODIFIER KEYS   ---   */
static const __int8 MOD_NONE = 1;

static const __int8 MOD_LALT    = 2;
static const __int8 MOD_RALT    = 3;

static const __int8 MOD_LCTRL   = 4;
static const __int8 MOD_RCTRL   = 5;

static const __int8 MOD_LSHIFT  = 6;
static const __int8 MOD_RSHIFT  = 7;

static const __int8 MOD_WINDOWS = 8;

// TODO IMPLEMENT MODIFIER ON KEYSTATE
/*   ---   END MODIFIER KEYS   ---   */

typedef struct key_state {
    bool pressed;
    bool released;
    bool held;
} key_state;

/*   ---   VARIABLES   ---   */

static key_state console_keys[256], console_mouse[5];
static short console_key_old_state[256] = {0};
static short console_key_new_state[256] = {0};
static bool console_mouse_old_state[5] = {0};
static bool console_mouse_new_state[5] = {0};
static bool console_in_focus = true;

/*   ---   FUNCTIONS   ---   */

boolean create_console(int width, int height, int font_w, int font_h,
                       void (*close_handler)(),
                       void (*create_handler)(),
                       void (*update)(float elapsed_time),
                       void (*render)()
                       );

void free_console();
void run_console();
void stop_console();
void set_console_title(wchar_t* title);
void error_exit(LPTSTR lpszFunction);

int console_height();
int console_width();
int console_mouse_x();
int console_mouse_y();

bool console_set_mouse_xy(int x, int y);
bool console_set_mouse_visible(bool visible);
bool console_set_mouse_size(int size);

key_state console_get_key(int vk);
key_state console_get_mouse(int vk);

/*   ---   UTILITIES   ---   */
void brensenham_line(vec *v, int x1, int y1, int x2, int y2);

/*   ---   DRAWING ROUTINES   ---   */
void console_clear(WCHAR chr, WORD attributes);
void console_draw(int x, int y, WCHAR chr, WORD attributes);
void console_line(int x1, int y1, int x2, int y2, WCHAR chr, WORD attributes);

#endif //CONSOLE_H
