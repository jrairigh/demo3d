#include "raylib.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

Font font = { 0 };

#define BG_COLOR CLITERAL(Color) { 50, 49, 64, 255 }

static const int screenWidth = 800;
static const int screenHeight = 450;

static void UpdateDrawFrame(void);

int main(void)
{
    InitWindow(screenWidth, screenHeight, "3d Demo");

    // Load global data (assets that must be available in all screens, i.e. font)
    font = LoadFont("resources/mecha.png");

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);

    // Detect window close button or ESC key
    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }
#endif

    // Unload global data loaded
    UnloadFont(font);

    // Close window and OpenGL context
    CloseWindow();

    return 0;
}

static void UpdateDrawFrame(void)
{
    BeginDrawing();

        ClearBackground(BG_COLOR);

    EndDrawing();
}
