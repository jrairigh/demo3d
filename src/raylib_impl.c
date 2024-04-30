#include "window.h"

#include "raylib.h"
#include "raymath.h"

static Font font = { 0 };
static int half_width = { 0 };
static int half_height = { 0 };

#define BG_COLOR CLITERAL(Color) { 50, 49, 64, 255 }

void raylib_show(const char* title, uint32_t viewport_width, uint32_t viewport_height)
{
    InitWindow(viewport_width, viewport_height, title);

    // Load global data (assets that must be available in all screens, i.e. font)
    font = LoadFont("resources/mecha.png");
    half_width = GetScreenWidth() / 2;
    half_height = GetScreenHeight() / 2;
    SetTargetFPS(60);
}

void raylib_on_update(Window* window, float ts)
{
    (void)ts;
    window->is_open = (int)!WindowShouldClose();
}

void raylib_begin_draw()
{
    BeginDrawing();
    ClearBackground(BG_COLOR);
}

void raylib_end_draw()
{
    EndDrawing();
}

void raylib_draw_pixel(float x, float y, MyColor color)
{
    const Color c = (Color){color.red, color.green, color.blue, color.alpha};
    const int nx = (int)((x + 1.0f) * half_width);
    const int ny = (int)((1.0f - y) * half_height);
    DrawPixel(nx, ny, c);
    DrawPixel(nx + 1, ny, c);
    DrawPixel(nx, ny + 1, c);
    DrawPixel(nx + 1, ny + 1, c);
    DrawPixel(nx - 1, ny, c);
    DrawPixel(nx, ny - 1, c);
    DrawPixel(nx - 1, ny - 1, c);
}

void raylib_close_window()
{
    // Unload global data loaded
    UnloadFont(font);

    // Close window and OpenGL context
    CloseWindow();
}
