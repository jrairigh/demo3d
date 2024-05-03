#include "window.h"

#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "styles/dark/style_dark.h"

static Font font = { 0 };
static const float font_size = 18.0f;
static const float font_spacing = 5.0f;
static const Color font_color = {0, 228, 48, 255};
static bool show_fps = false;
static bool collapse_control_window = false;

#define BG_COLOR CLITERAL(Color) { 50, 49, 64, 255 }

// GUI design variables for positioning and sizing components
//float x = 0.0f, y = 0.0f, width = 50.0f, height = 20.0f;
const float control_window_screen_width_percentage = 0.27f;
float control_window_height;
float control_window_width;

static Rectangle rect(const float x, const float y, const float width, const float height)
{
    return (Rectangle) { x, y, width, height };
}

static void draw_info();
static void draw_controls();

void raylib_show(const char* title, const uint32_t screen_width, const uint32_t screen_height)
{
    InitWindow(screen_width, screen_height, title);

    // Load global data (assets that must be available in all screens, i.e. font)
    font = LoadFont("resources/mecha.png");
    //SetTargetFPS(60);
    GuiLoadStyleDark();
    //SetConfigFlags(FLAG_WINDOW_UNDECORATED);

    control_window_height = 1.0f * GetScreenHeight();
    control_window_width = control_window_screen_width_percentage * GetScreenWidth();
}

float raylib_viewport_width()
{
    return (1.0f - (float)(!collapse_control_window) * control_window_screen_width_percentage) * GetScreenWidth();
}

float raylib_viewport_height()
{
    return (float)GetScreenHeight();
}

void raylib_on_update(Window* window, const float ts)
{
    (void)ts;
    window->is_open = (int)!WindowShouldClose();

    if (IsKeyPressed(KEY_I))
    {
        show_fps = !show_fps;
    }
}

void raylib_begin_draw()
{
    BeginDrawing();
    ClearBackground(BG_COLOR);

    if (show_fps)
        draw_info();
    else
        DrawTextEx(font, "Show info 'I' key", Vector2Scale(Vector2One(), 2.0f), font_size, font_spacing, font_color);

    draw_controls();
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
    const float viewport_half_width = raylib_viewport_width() / 2.0f;
    const float viewport_half_height = raylib_viewport_height() / 2.0f;
    const int viewport_space_x = (int)((x + 1.0f) * viewport_half_width);
    const int viewport_space_y = (int)((1.0f - y) * viewport_half_height);

    // drawing extra pixels to fill in gaps
    DrawPixel(viewport_space_x - 1, viewport_space_y - 1, c);
    DrawPixel(viewport_space_x,     viewport_space_y - 1, c);
    DrawPixel(viewport_space_x + 1, viewport_space_y - 1, c);

    DrawPixel(viewport_space_x - 1, viewport_space_y, c);
    DrawPixel(viewport_space_x,     viewport_space_y, c);
    DrawPixel(viewport_space_x + 1, viewport_space_y, c);

    DrawPixel(viewport_space_x - 1, viewport_space_y + 1, c);
    DrawPixel(viewport_space_x,     viewport_space_y + 1, c);
    DrawPixel(viewport_space_x + 1, viewport_space_y + 1, c);
}

void raylib_close_window()
{
    // Unload global data loaded
    UnloadFont(font);

    // Close window and OpenGL context
    CloseWindow();
}

static void draw_controls()
{
    if (collapse_control_window)
    {
        collapse_control_window ^= GuiWindowBox(rect(GetScreenWidth() - 10.0f, 0.0f, 10.0f, 1.0f * GetScreenHeight()), "");
        return;
    }

    const float control_window_x = GetScreenWidth() * (1.0f - control_window_screen_width_percentage);
    const float control_window_y = 0.0f;

    const float fov_slider_height = 22.0f;
    const float fov_slider_width = 113.0f;
    const float fov_slider_x = 639.0f;
    const float fov_slider_y = 39.0f;

    const float near_z_slider_height = 22.0f;
    const float near_z_slider_width = 113.0f;
    const float near_z_slider_x = 639.0f;
    const float near_z_slider_y = 66.0f;

    const float far_z_slider_height = 22.0f;
    const float far_z_slider_width = 113.0f;
    const float far_z_slider_x = 639.0f;
    const float far_z_slider_y = 93.0f;

    collapse_control_window = GuiWindowBox(rect(control_window_x, control_window_y, control_window_width, control_window_height), "Controls");

    // GUI Design controls for positioning and sizing components
    //GuiSlider(rect(30, 420, 200, 30), "x", TextFormat("%2.2f", x), &x, 0.0f, (float)GetScreenWidth());
    //GuiSlider(rect(30, 460, 200, 30), "y", TextFormat("%2.2f", y), &y, 0.0f, (float)GetScreenWidth());
    //GuiSlider(rect(30, 500, 200, 30), "width", TextFormat("%2.2f", width), &width, 0.0f, (float)GetScreenWidth());
    //GuiSlider(rect(30, 540, 200, 30), "height", TextFormat("%2.2f", height), &height, 0.0f, (float)GetScreenWidth());

    GuiSlider(rect(fov_slider_x, fov_slider_y, fov_slider_width, fov_slider_height), "FOV", TextFormat("%2.0f", g_fov), &g_fov, 1.0f, 100.0f);
    GuiSlider(rect(near_z_slider_x, near_z_slider_y, near_z_slider_width, near_z_slider_height), "Near Z", TextFormat("%2.1f", g_near_z), &g_near_z, 1, 100.0f);
    GuiSlider(rect(far_z_slider_x, far_z_slider_y, far_z_slider_width, far_z_slider_height), "Far Z", TextFormat("%2.1f", g_far_z), &g_far_z, 100.0f, 999.9f);
}

static void draw_info()
{
    const float width = raylib_viewport_width();
    const float height = raylib_viewport_height();
    DrawFPS(2, GetScreenHeight() - 20);

    Vector2 position = {2.0f, 2.0f};
    DrawTextEx(font, TextFormat("Viewport <%2.0fpx, %2.0fpx>", width, height), position, font_size, font_spacing, font_color);
}