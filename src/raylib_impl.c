#include "window.h"

#include "raylib.h"
#include "raymath.h"

static Font font = { 0 };
static float half_width = 0.0f;
static float half_height = 0.0f;

#define BG_COLOR CLITERAL(Color) { 50, 49, 64, 255 }

void raylib_show(const char* title, const uint32_t viewport_width, const uint32_t viewport_height)
{
    InitWindow(viewport_width, viewport_height, title);

    // Load global data (assets that must be available in all screens, i.e. font)
    font = LoadFont("resources/mecha.png");
    half_width = GetScreenWidth() / 2.0f;
    half_height = GetScreenHeight() / 2.0f;
    SetTargetFPS(60);
}

void raylib_on_update(Window* window, const float ts)
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

void raylib_draw_pixel(const Vec2 normalized_coordinate, const MyColor color)
{
    const float x = normalized_coordinate.x;
    const float y = normalized_coordinate.y;
    const Color c = (Color){color.red, color.green, color.blue, color.alpha};
    const int screen_space_x = (int)((x + 1.0f) * half_width);
    const int screen_space_y = (int)((1.0f - y) * half_height);

    // drawing extra pixels to fill in gaps
    DrawPixel(screen_space_x - 1, screen_space_y - 1, c);
    DrawPixel(screen_space_x,     screen_space_y - 1, c);
    DrawPixel(screen_space_x + 1, screen_space_y - 1, c);

    DrawPixel(screen_space_x - 1, screen_space_y, c);
    DrawPixel(screen_space_x,     screen_space_y, c);
    DrawPixel(screen_space_x + 1, screen_space_y, c);

    DrawPixel(screen_space_x - 1, screen_space_y + 1, c);
    DrawPixel(screen_space_x,     screen_space_y + 1, c);
    DrawPixel(screen_space_x + 1, screen_space_y + 1, c);
}

void raylib_close_window()
{
    // Unload global data loaded
    UnloadFont(font);

    // Close window and OpenGL context
    CloseWindow();
}
