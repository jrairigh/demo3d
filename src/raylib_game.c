#include "window.h"

#include "raylib.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

Font font = { 0 };

#define BG_COLOR CLITERAL(Color) { 50, 49, 64, 255 }

void raylib_show(const char* title, uint32_t viewport_width, uint32_t viewport_height)
{
    InitWindow(viewport_width, viewport_height, title);

    // Load global data (assets that must be available in all screens, i.e. font)
    font = LoadFont("resources/mecha.png");
}

void raylib_on_update(Window* window, float ts)
{
    window->is_open = (int)!WindowShouldClose();
}

void raylib_draw_pixel(float x, float y, MyColor color)
{
    Color c = (Color){color.red, color.green, color.blue, color.alpha};
    BeginDrawing();

    ClearBackground(BG_COLOR);
    DrawPixel((int)x, (int)y, c);

    EndDrawing();
}

void raylib_close_window()
{
    // Unload global data loaded
    UnloadFont(font);

    // Close window and OpenGL context
    CloseWindow();
}
