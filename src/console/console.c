#include "../include/console/console.h"

/*   ---   PRIVATE   ---   */
bool c_should_close = false;
Console* c_console = NULL;
COORD c_mouse_pos = {
        (short) 0,
        (short) 0
};

double PCFreq = 0.0;
__int64 CounterStart = 0;

void start_counter()
{
    LARGE_INTEGER li;
    if(!QueryPerformanceFrequency(&li))
        error_exit(L"QueryPerformanceFrequency failed!\n");

    PCFreq = (double)(li.QuadPart);

    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
}

// gets ms passed from counter
double get_counter()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return (double)(li.QuadPart-CounterStart)/PCFreq;
}

BOOL ctrl_handler(DWORD dwCtrlType) {
    // do not care about ctrl type, all that is important is that we must close the application
    c_should_close = true;
    return TRUE;
}

/*   ---   PUBLIC   ---   */

// see: https://docs.microsoft.com/en-us/windows/win32/debug/retrieving-the-last-error-code
void error_exit(LPTSTR lpszFunction)
{
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0, NULL );

    // Display the error message and exit the process

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
                                      (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
                    LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                    TEXT("%s failed with error %d: %s"),
                    lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
    ExitProcess(dw);
}

boolean create_console(int width, int height, int font_w, int font_h,
                       void (*close_handler)(),
                       void (*create_handler)(),
                       void (*update)(float elapsed_time),
                       void (*render)()) {

    if (c_console != NULL) return false; // console already created

    c_console = (Console*) malloc(sizeof(Console));
    if (c_console == NULL) {
        error_exit(L"create_console(): Out Of Memory");
    }

    HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h_console == INVALID_HANDLE_VALUE) {
        error_exit(L"create_console(): GetStdHandle(STD_OUTPUT_HANDLE)");
    }

    HANDLE h_in = GetStdHandle(STD_INPUT_HANDLE);
    if (h_in == INVALID_HANDLE_VALUE) {
        error_exit(L"create_console(): GetStdHandle(STD_INPUT_HANDLE)");
    }

    c_console->h_in = h_in;
    c_console->h_console = h_console;

    memset(console_key_new_state, 0, 256 * sizeof(short));
    memset(console_key_old_state, 0, 256 * sizeof(short));
    memset(console_keys, 0, 256 * sizeof(key_state));

    memset(console_mouse_new_state, false, 5 * sizeof(bool));
    memset(console_mouse_old_state, false, 5 * sizeof(bool));
    memset(console_mouse, false, 5 * sizeof(key_state));

    SMALL_RECT rect_window = {0, 0, 1, 1};
    SetConsoleWindowInfo(c_console->h_console, TRUE, &rect_window);

    // set size of screen buffer
    c_console->width = width;
    c_console->height = height;
    COORD coord = { (short) width, (short) height};
    if (!SetConsoleScreenBufferSize(c_console->h_console, coord)) {
        error_exit(L"create_console(): SetConsoleScreenBufferSize()");
    }

    // assign screen buffer to console
    if (!SetConsoleActiveScreenBuffer(c_console->h_console)) {
        error_exit(L"create_console(): SetConsoleActiveScreenBuffer()");
    }

    // set font size
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = font_w;
    cfi.dwFontSize.Y = font_h;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;

    wcscpy_s(cfi.FaceName, _countof(L"Consolas"), L"Consolas");

    if (!SetCurrentConsoleFontEx(c_console->h_console, false, &cfi)) {
        error_exit(L"create_console(): SetCurrentConsoleFontEx()");
    }

    c_console->font_w = font_w;
    c_console->font_h = font_h;

    // check that given window size does not exceed the maximum allowed window size
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if(!GetConsoleScreenBufferInfo(c_console->h_console, &csbi)) {
        error_exit(L"create_console(): GetConsoleScreenBufferInfo()");
    }

    if (width > csbi.dwMaximumWindowSize.X) {
        error_exit(L"create_console(): width is greater than maximum window width");
    }

    if (height > csbi.dwMaximumWindowSize.Y) {
        error_exit(L"create_console(): height is greater than maximum window height");
    }

    // set physical console window size
    rect_window.Right = (short)width-1;
    rect_window.Bottom = (short)height-1;
    if (!SetConsoleWindowInfo(c_console->h_console, TRUE, &rect_window)) {
        error_exit(L"create_console(): SetConsoleWindowInfo()");
    }

    c_console->rect_window = rect_window;

    // set flags for mouse input
    if (!SetConsoleMode(c_console->h_in, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT)) {
        error_exit(L"create_console(): SetConsoleMode()");
    }

    // allocate screen buffer memory
    c_console->scr_buff = (CHAR_INFO*) malloc(sizeof(CHAR_INFO) * width * height);
    if (c_console->scr_buff == NULL) {
        error_exit(L"create_console(): Out Of Memory");
    }

    c_console->clear_color = FG_WHITE | BG_BLACK;
    c_console->clear_char = U_NULL;

    if(!SetConsoleCtrlHandler(ctrl_handler, TRUE)) {
        error_exit(L"SetConsoleCtrlHandler");
    }

    // set cursor info
    // dwSize specifies percentage of cell filled by the cursor (0 - 100)
    c_console->cursor_info.dwSize = 25;
    c_console->cursor_info.bVisible = false;
    if(!SetConsoleCursorInfo(c_console->h_console, &c_console->cursor_info)) {
        error_exit(L"create_console(): SetConsoleCursorInfo");
    }

    if (!SetConsoleCursorPosition(c_console->h_console, c_mouse_pos)) {
        error_exit(L"create_console(): SetConsoleCursorPosition");
    }

    // set handlers
    if (close_handler == NULL) {
        error_exit(L"create_console(): missing close handler");
    }

    if (create_handler == NULL) {
        error_exit(L"create_console(): missing create handler");
    }

    if (render == NULL) {
        error_exit(L"create_console(): missing render handler");
    }

    if (update == NULL) {
        error_exit(L"create_console(): missing update handler");
    }

    c_console->on_close = close_handler;
    c_console->on_create = create_handler;
    c_console->render = render;
    c_console->update = update;

    c_console->on_create();
    return true;
}

void free_console() {
    if (c_console->on_close != NULL) {
        c_console->on_close(c_console);
    }
    free(c_console->scr_buff);
    free(c_console);
}

void run_console() {
    if (c_console->update == NULL) {
        error_exit(L"run_console(): missing update function");
    }

    if (c_console->render == NULL) {
        error_exit(L"run_console(): missing render function");
    }

    // see: https://docs.microsoft.com/en-us/windows/win32/api/timeapi/nf-timeapi-timegettime
    start_counter();
    double t1, t2, elapsed;
    t1 = get_counter();
    t2 = t1;

    while(c_should_close == false) {
        t2 = get_counter();
        elapsed = t2 - t1;
        t1 = t2;

        // handle keyboard input
        for (int i = 0; i < 256; i++) {
            console_key_new_state[i] = GetAsyncKeyState(i);
            console_keys[i].pressed = false;
            console_keys[i].released = false;

            if (console_key_new_state[i] != console_key_old_state[i]) {
                if (console_key_new_state[i] & 0x8000) {
                    console_keys[i].pressed = !console_keys[i].held;
                    console_keys[i].held = true;
                } else {
                    console_keys[i].released = true;
                    console_keys[i].held = false;
                }
            }

            console_key_old_state[i] = console_key_new_state[i];
        }

        // handle mouse input
        INPUT_RECORD in_buff[32];
        DWORD events = 0;
        GetNumberOfConsoleInputEvents(c_console->h_in, &events);
        if (events > 0) {
            ReadConsoleInput(c_console->h_in, in_buff, events, &events);
        }

        // handle events
        for (DWORD i = 0; i < events; i++) {
            switch(in_buff[i].EventType) {
                case FOCUS_EVENT:
                    console_in_focus = in_buff[i].Event.FocusEvent.bSetFocus;
                    break;

                case MOUSE_EVENT:
                    switch(in_buff[i].Event.MouseEvent.dwEventFlags) {
                        case MOUSE_MOVED:
                            console_set_mouse_xy(
                                    in_buff[i].Event.MouseEvent.dwMousePosition.X,
                                    in_buff[i].Event.MouseEvent.dwMousePosition.Y
                                    );
                            break;

                        case 0:
                            for (int m = 0; m < 5; m++) {
                                console_mouse_new_state[m] = (in_buff[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;
                            }
                            break;

                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
        }

        for(int m = 0; m < 5; m++) {
            console_mouse[m].pressed = false;
            console_mouse[m].released = false;

            if (console_mouse_new_state[m] != console_mouse_old_state[m]) {
                if (console_mouse_new_state[m]) {
                    console_mouse[m].pressed = true;
                    console_mouse[m].held = true;
                } else {
                    console_mouse[m].released = true;
                    console_mouse[m].held = false;
                }
            }

            console_mouse_old_state[m] = console_mouse_new_state[m];
        }

        // call user update and render functions
        c_console->update(elapsed);
        c_console->render();

        // update screen buffer
        WriteConsoleOutput(c_console->h_console, c_console->scr_buff, (COORD){(short) c_console->width, (short) c_console->height},
                           (COORD){0, 0}, &(c_console->rect_window));
    }
}

void stop_console() {
    c_should_close = true;
}

int console_width() {
    return c_console->width;
}

int console_height() {
    return c_console->height;
}

int console_mouse_x() {
    return c_mouse_pos.X;
}

int console_mouse_y() {
    return c_mouse_pos.Y;
}

bool console_set_mouse_xy(int x, int y) {
    bool res;
    COORD new = {
            (short) x,
            (short) y,
    };
    res = SetConsoleCursorPosition(c_console->h_console, new);
    if (res == true) {
        c_mouse_pos = new;
    }

    return res;
}

bool console_set_mouse_visible(bool visible) {
    CONSOLE_CURSOR_INFO new = {
            c_console->cursor_info.dwSize,
            visible
    };

    bool res;
    res = SetConsoleCursorInfo(c_console->h_console, &new);
    if (res == true) {
        c_console->cursor_info = new;
    }

    return res;
}

bool console_set_mouse_size(int size) {
    CONSOLE_CURSOR_INFO new = {
            size,
            c_console->cursor_info.bVisible,
    };

    bool res;
    res = SetConsoleCursorInfo(c_console->h_console, &new);
    if (res == true) {
        c_console->cursor_info = new;
    }

    return res;
}

key_state console_get_key(int vk) {
    return console_keys[vk];
}

key_state console_get_mouse(int vk) {
    return console_mouse[vk];
}

void set_console_title(wchar_t* title) {
    if(!SetConsoleTitle(title)) {
        error_exit(L"SetConsoleTitle");
    }
}

/*   ---   UTILITIES   ---   */

void brensenham_line(vec *v, int x1, int y1, int x2, int y2) {
    double dx = abs(x2-x1);
    double sx = (x1 < x2) ? 1 : -1;
    double dy = -abs(y2-y1);
    double sy = (y1 < y2) ? 1 : -1;
    double err = dx + dy;
    double e2 = 0;
    while (true) {
        Point* p = malloc(sizeof(Point));
        p->x = x1;
        p->y = y1;
        vec_add(v, (void *) p);

        if (x1 == x2 && y1 == y2) break;

        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x1 += sx;
        }

        if (e2 <= dx) {
            err += dx;
            y1 += sy;
        }
    }
}

/*   ---   DRAWING ROUTINES   ---   */

void console_draw(int x, int y, WCHAR chr, WORD attributes) {
    if (x < 0 || x >= c_console->width || y < 0 || y >= c_console->height) return;

    c_console->scr_buff[y * c_console->width + x].Char.UnicodeChar = chr;
    c_console->scr_buff[y * c_console->width + x].Attributes = attributes;
}

void console_clear(WCHAR chr, WORD attributes) {
    for(int i = 0; i < c_console->width * c_console->height; i++) {
        c_console->scr_buff[i].Char.UnicodeChar = chr;
        c_console->scr_buff[i].Attributes = attributes;
    }
}

void console_line(int x1, int y1, int x2, int y2, WCHAR chr, WORD attributes) {
    int estimated_capacity = (int) ceil(sqrt(pow((x2-x1), 2) + pow((y2-1), 2)));
    vec points = vec_create(max(estimated_capacity, 1));

    // brensenham line
    brensenham_line(&points, x1, y1, x2, y2);
    for (int i = 0; i < points.size; i++) {
        Point* p = (Point*) points.arr[i];
        console_draw(p->x, p->y, chr, attributes);
        free(p);
    }

    vec_free(&points);
}

void console_rect(int x, int y, int w, int h, WCHAR chr, WORD attributes) {
    for (int px = x; px < x + w; px++) {
        console_draw(px, y, chr, attributes);
        console_draw(px, y+h-1, chr, attributes);
    }

    for (int py = y; py < y + h; py++) {
        console_draw(x, py, chr, attributes);
        console_draw(x+w-1, py, chr, attributes);
    }
}

void console_rect_fill(int x, int y, int w, int h, WCHAR chr, WORD attributes) {
    console_rect(x, y, w, h, chr, attributes);
    for (int px = x+1; px < x + w - 1; px++) {
        for (int py = y+1; py < y + h - 1; py++) {
            console_draw(px, py, chr, attributes);
        }
    }
}

void console_ellipse(int rx, int ry, int xc, int yc, WCHAR chr, WORD attributes)
{
    float dx, dy, d1, d2, x, y;
    x = 0;
    y = ry;

    // Initial decision parameter of region 1
    d1 = (ry * ry)
         - (rx * rx * ry)
         + (0.25 * rx * rx);
    dx = 2 * ry * ry * x;
    dy = 2 * rx * rx * y;

    // For region 1
    while (dx < dy) {

        // Print points based on 4-way symmetry
        console_draw(x + xc, y + yc, chr, attributes);
        console_draw(-x + xc, y + yc, chr, attributes);
        console_draw(x + xc, -y + yc, chr, attributes);
        console_draw(-x + xc, -y + yc, chr, attributes);

        // Checking and updating value of
        // decision parameter based on algorithm
        if (d1 < 0) {
            x++;
            dx = dx + (2 * ry * ry);
            d1 = d1 + dx + (ry * ry);
        }
        else {
            x++;
            y--;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d1 = d1 + dx - dy + (ry * ry);
        }
    }

    // Decision parameter of region 2
    d2 = ((ry * ry) * ((x + 0.5) * (x + 0.5)))
         + ((rx * rx) * ((y - 1) * (y - 1)))
         - (rx * rx * ry * ry);

    // Plotting points of region 2
    while (y >= 0) {

        // printing points based on 4-way symmetry
        console_draw(x + xc, y + yc, chr, attributes);
        console_draw(-x + xc, y + yc, chr, attributes);
        console_draw(x + xc, -y + yc, chr, attributes);
        console_draw(-x + xc, -y + yc, chr, attributes);

        // Checking and updating parameter
        // value based on algorithm
        if (d2 > 0) {
            y--;
            dy = dy - (2 * rx * rx);
            d2 = d2 + (rx * rx) - dy;
        }
        else {
            y--;
            x++;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d2 = d2 + dx - dy + (rx * rx);
        }
    }
}

void console_ellipse_fill(int rx, int ry, int xc, int yc, WCHAR chr, WORD attributes) {
    float dx, dy, d1, d2, x, y;
    x = 0;
    y = ry;

    // Initial decision parameter of region 1
    d1 = (ry * ry)
         - (rx * rx * ry)
         + (0.25 * rx * rx);
    dx = 2 * ry * ry * x;
    dy = 2 * rx * rx * y;

    // For region 1
    while (dx < dy) {

        // Print points based on 4-way symmetry
        console_line(x + xc, y + yc, -x + xc, y + yc, chr, attributes);
        console_line(x + xc, -y + yc, -x + xc, -y + yc, chr, attributes);

        // Checking and updating value of
        // decision parameter based on algorithm
        if (d1 < 0) {
            x++;
            dx = dx + (2 * ry * ry);
            d1 = d1 + dx + (ry * ry);
        }
        else {
            x++;
            y--;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d1 = d1 + dx - dy + (ry * ry);
        }
    }

    // Decision parameter of region 2
    d2 = ((ry * ry) * ((x + 0.5) * (x + 0.5)))
         + ((rx * rx) * ((y - 1) * (y - 1)))
         - (rx * rx * ry * ry);

    // Plotting points of region 2
    while (y >= 0) {

        // printing points based on 4-way symmetry
        console_line(x + xc, y + yc, -x + xc, y + yc, chr, attributes);
        console_line(x + xc, -y + yc, -x + xc, -y + yc, chr, attributes);

        // Checking and updating parameter
        // value based on algorithm
        if (d2 > 0) {
            y--;
            dy = dy - (2 * rx * rx);
            d2 = d2 + (rx * rx) - dy;
        }
        else {
            y--;
            x++;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d2 = d2 + dx - dy + (rx * rx);
        }
    }
}

void console_circle(int r, int xc, int yc, WCHAR chr, WORD attributes) {
    console_ellipse(r, r, xc, yc, chr, attributes);
}

void console_circle_fill(int r, int xc, int yc, WCHAR chr, WORD attributes) {
    console_ellipse_fill(r, r, xc, yc, chr, attributes);
}

/*   ---   UI ROUTINES   ---   */

bool is_flag_set(int data, int flag) {
    return ((data & flag) == flag);
}

void render_border(Component *component,
        wchar_t horizontal, wchar_t vertical, wchar_t top_left_corner, wchar_t top_right_corner,
        wchar_t bottom_left_corner, wchar_t bottom_right_corner) {
    for (int x = component->x + 1; x < component->x + component->w - 1; x++) {
        console_draw(x, component->y, horizontal, component->attributes);
        console_draw(x, component->y+component->h-1, horizontal, component->attributes);
    }

    for (int y = component->y + 1; y < component->y + component->h - 1; y++) {
        console_draw(component->x, y, vertical, component->attributes);
        console_draw(component->x + component->w - 1, y, vertical, component->attributes);
    }

    console_draw(component->x, component->y, top_left_corner, component->attributes);
    console_draw(component->x + component->w - 1, component->y, top_right_corner, component->attributes);

    console_draw(component->x, component->y + component->h - 1, bottom_left_corner, component->attributes);
    console_draw(component->x + component->w - 1, component->y + component->h - 1, bottom_right_corner, component->attributes);
}

void render_title(Component *component) {
    int alignment_flags = component->alignments;
    int decoration_flags = component->decorations;
    Point left_cap = {0, 0};
    Point right_cap = {0, 0};

    int l = lstrlenW(component->title);
    if (l > component->w) {
        l = l - (l - component->w) - 2;
    }

    // TODO: FIX TITLE CAPS
    // TODO: ADD TITLE CAPS FOR BOTTOM ALIGNMENTS
    if (is_flag_set(alignment_flags, TitleTopCenter)) {
        int left_pad = ceil((double)component->w/2) - ceil((double)l/2);
        left_cap.x = component->x + left_pad - 1;
        left_cap.y = component->y;
        right_cap.x = component->x + left_pad + l;
        right_cap.y = component->y;

        for (int i = 0; i < l; i++) {
            console_draw(component->x + left_pad + i, component->y, component->title[i], component->attributes);
        }
    }

    if (is_flag_set(alignment_flags, TitleTopLeft)) {
        left_cap.x = component->x;
        left_cap.y = component->y;
        right_cap.x = component->x + l + 1;
        right_cap.y = component->y;

        for (int i = 0; i < l; i++) {
            console_draw(component->x + 1 + i, component->y, component->title[i], component->attributes);
        }
    }

    if (is_flag_set(alignment_flags, TitleTopRight)) {
        left_cap.x = component->x + component->w - l - 2;
        left_cap.y = component->y;
        right_cap.x = component->x + component->w - 1;
        right_cap.y = component->y;

        for (int i = 0; i < l; i++) {
            console_draw(component->x + component->w - 2 - i, component->y, component->title[l-1-i], component->attributes);
        }
    }

    if (is_flag_set(alignment_flags, TitleBottomCenter)) {
        int left_pad = ceil((double)component->w/2) - ceil((double)l/2);

        for (int i = 0; i < l; i++) {
            console_draw(component->x + left_pad + i, component->y + component->h - 1, component->title[i], component->attributes);
        }
    }

    if (is_flag_set(alignment_flags, TitleBottomLeft)) {
        for (int i = 0; i < l; i++) {
            console_draw(component->x + 1 + i, component->y  + component->h - 1, component->title[i], component->attributes);
        }
    }

    if (is_flag_set(alignment_flags, TitleBottomRight)) {
        for (int i = 0; i < l; i++) {
            console_draw(component->x + component->w - 2 - i, component->y  + component->h - 1, component->title[l-1-i], component->attributes);
        }
    }

    // end caps on title if there is a border
    if (is_flag_set(decoration_flags, SimpleBorder)) {
        console_draw(left_cap.x, left_cap.y, L'|', component->attributes);
        console_draw(right_cap.x, right_cap.y, L'|', component->attributes);
    }

    if (is_flag_set(decoration_flags, SingleBorder)) {
        console_draw(left_cap.x, left_cap.y, U_LINE_LEFT_CAP_SINGLE, component->attributes);
        console_draw(right_cap.x, right_cap.y, U_LINE_RIGHT_CAP_SINGLE, component->attributes);
    }

    if (is_flag_set(decoration_flags, DoubleBorder)) {
        console_draw(left_cap.x, left_cap.y, U_LINE_LEFT_CAP_DOUBLE, component->attributes);
        console_draw(right_cap.x, right_cap.y, U_LINE_RIGHT_CAP_DOUBLE, component->attributes);
    }
}

void default_component_renderer(Component *component) {
    for (int x = component->x; x < component->x + component->w; x++) {
        for (int y = component->y; y < component->y + component->h; y++) {
            console_draw(x, y, U_NULL, component->attributes);
        }
    }

    // render decorations
    int dec_flags = component->decorations;

    if (is_flag_set(dec_flags, SingleBorder)) {
        render_border(component, U_LINE_HORZ_SINGLE, U_LINE_VERT_SINGLE, U_LINE_TOP_LEFT_SINGLE,
                U_LINE_TOP_RIGHT_SINGLE, U_LINE_BOTTOM_LEFT_SINGLE, U_LINE_BOTTOM_RIGHT_SINGLE);
    }

    if (is_flag_set(dec_flags, DoubleBorder)) {
        render_border(component, U_LINE_HORZ_DOUBLE, U_LINE_VERT_DOUBLE, U_LINE_TOP_LEFT_DOUBLE,
                      U_LINE_TOP_RIGHT_DOUBLE, U_LINE_BOTTOM_LEFT_DOUBLE, U_LINE_BOTTOM_RIGHT_DOUBLE);
    }

    if (is_flag_set(dec_flags, SimpleBorder)) {
        render_border(component, L'-', L'|', L'+', L'+', L'+', L'+');
    }

    // render title
    if (component->title != NULL) {
        render_title(component);
    }
}

void default_panel_renderer(Panel *panel) {
    panel->component.render(&panel->component);
}

Component create_component(int x, int y, int w, int h, wchar_t* title) {
    Component c = {0};
    c.x = x;
    c.y = y;
    c.w = w;
    c.h = h;
    c.title = title;
    c.attributes = FG_WHITE | BG_BLACK;
    c.alignments = TitleTopLeft;
    c.decorations = SingleBorder;
    c.render = default_component_renderer;
    return c;
}

void set_component_renderer(Component *component, void (*render)(Component *component)) {
    component->render = render;
}

Panel create_panel(int x, int y, int w, int h, wchar_t* title) {
    Panel p = {0};
    p.component = create_component(x, y, w, h, title);
    p.render = default_panel_renderer;
    return p;
}

void set_panel_renderer(Panel *panel, void (*render)(Panel *panel)) {
    panel->render = render;
}
