#include "window.h"

#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "styles/dark/style_dark.h"

#include <assert.h>

static Font font = { 0 };
static Image image_buffer;
static const float font_size = 18.0f;
static const float font_spacing = 5.0f;
static const Color font_color = {0, 228, 48, 255};
static bool show_fps = false;
static bool collapse_control_window = false;

#define BG_COLOR CLITERAL(Color) { 50, 49, 64, 255 }
#define TRANSPARENT CLITERAL(Color) {0, 0, 0, 0}

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

uint32_t raylib_viewport_width()
{
    return (uint32_t)((1.0f - (float)(!collapse_control_window) * control_window_screen_width_percentage) * GetScreenWidth());
}

uint32_t raylib_viewport_height()
{
    return (uint32_t)GetScreenHeight();
}

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

    image_buffer = GenImageWhiteNoise((int)raylib_viewport_width(), (int)raylib_viewport_height(), 1.0f);
}

void raylib_on_update(Window* window, const float ts)
{
    (void)ts;
    window->is_open = (int)!WindowShouldClose();

    if (IsKeyPressed(KEY_I))
    {
        show_fps = !show_fps;
    }

    window->wasd_key_state[0] = IsKeyPressedRepeat(KEY_W);
    window->wasd_key_state[1] = IsKeyPressedRepeat(KEY_A);
    window->wasd_key_state[2] = IsKeyPressedRepeat(KEY_S);
    window->wasd_key_state[3] = IsKeyPressedRepeat(KEY_D);
}

void raylib_begin_draw()
{
    BeginDrawing();
    ClearBackground(BG_COLOR);
}

void raylib_end_draw()
{
    // Drawing overlays last so viewport window doesn't render graphics overtop of it
    if (show_fps)
        draw_info();
    else
        DrawTextEx(font, "Show info 'I' key", Vector2Scale(Vector2One(), 2.0f), font_size, font_spacing, font_color);

    draw_controls();

    EndDrawing();
}

void raylib_render()
{
    Texture2D texture = LoadTextureFromImage(image_buffer);
    DrawTexture(texture, 0, 0, TRANSPARENT);
    UnloadTexture(texture);
}

void raylib_draw_pixel(const Vec2 normalized_coordinate, const MyColor color)
{
    const float x = normalized_coordinate.x;
    const float y = normalized_coordinate.y;
    const Color c = {color.red, color.green, color.blue, color.alpha};
    const float viewport_half_width = raylib_viewport_width() / 2.0f;
    const float viewport_half_height = raylib_viewport_height() / 2.0f;
    const int viewport_space_x = (int)((x + 1.0f) * viewport_half_width);
    const int viewport_space_y = (int)((1.0f - y) * viewport_half_height);

    // drawing extra pixels to fill in gaps
    DrawPixel(viewport_space_x - 1, viewport_space_y - 1, c);
    DrawPixel(viewport_space_x, viewport_space_y - 1, c);
    DrawPixel(viewport_space_x + 1, viewport_space_y - 1, c);

    DrawPixel(viewport_space_x - 1, viewport_space_y, c);
    DrawPixel(viewport_space_x, viewport_space_y, c);
    DrawPixel(viewport_space_x + 1, viewport_space_y, c);

    DrawPixel(viewport_space_x - 1, viewport_space_y + 1, c);
    DrawPixel(viewport_space_x, viewport_space_y + 1, c);
    DrawPixel(viewport_space_x + 1, viewport_space_y + 1, c);

    //ImageDrawPixel(&image_buffer, viewport_space_x - 1, viewport_space_y - 1, c);
    //ImageDrawPixel(&image_buffer, viewport_space_x,     viewport_space_y - 1, c);
    //ImageDrawPixel(&image_buffer, viewport_space_x + 1, viewport_space_y - 1, c);
    //
    //ImageDrawPixel(&image_buffer, viewport_space_x - 1, viewport_space_y, c);
    //ImageDrawPixel(&image_buffer, viewport_space_x,     viewport_space_y, c);
    //ImageDrawPixel(&image_buffer, viewport_space_x + 1, viewport_space_y, c);
    //
    //ImageDrawPixel(&image_buffer, viewport_space_x - 1, viewport_space_y + 1, c);
    //ImageDrawPixel(&image_buffer, viewport_space_x,     viewport_space_y + 1, c);
    //ImageDrawPixel(&image_buffer, viewport_space_x + 1, viewport_space_y + 1, c);
}

void raylib_draw_line(const Vec2 start, const Vec2 end, const MyColor color)
{
    const float sx = start.x;
    const float sy = start.y;
    const float ex = end.x;
    const float ey = end.y;
    const Color c = { color.red, color.green, color.blue, color.alpha };
    const float viewport_half_width = raylib_viewport_width() * 0.5f;
    const float viewport_half_height = raylib_viewport_height() * 0.5f;
    const int viewport_space_sx = (int)((sx + 1.0f) * viewport_half_width);
    const int viewport_space_sy = (int)((1.0f - sy) * viewport_half_height);
    const int viewport_space_ex = (int)((ex + 1.0f) * viewport_half_width);
    const int viewport_space_ey = (int)((1.0f - ey) * viewport_half_height);

    DrawLine(viewport_space_sx, viewport_space_sy, viewport_space_ex, viewport_space_ey, c);
}

void raylib_draw_overlay_text(const char* text, const Vec2 position, const MyColor color)
{
    const Color c = { color.red, color.green, color.blue, color.alpha };
    const Vector2 p = { position.x, position.y };
    DrawTextEx(font, text, p, font_size, font_spacing, c);
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

    const float x_slider_height = 22.0f;
    const float x_slider_width = 113.0f;
    const float x_slider_x = 639.0f;
    const float x_slider_y = 120.0f;

    const float y_slider_height = 22.0f;
    const float y_slider_width = 113.0f;
    const float y_slider_x = 639.0f;
    const float y_slider_y = 147.0f;

    const float z_slider_height = 22.0f;
    const float z_slider_width = 113.0f;
    const float z_slider_x = 639.0f;
    const float z_slider_y = 174.0f;

    const float orthographic_mode_checkbox_height = 20.0f;
    const float orthographic_mode_checkbox_width = 20.0f;
    const float orthographic_mode_checkbox_x = 639.0f;
    const float orthographic_mode_checkbox_y = 201.0f;

    collapse_control_window = GuiWindowBox(rect(control_window_x, control_window_y, control_window_width, control_window_height), "Controls");

    // GUI Design controls for positioning and sizing components
    //GuiSlider(rect(30, 420, 200, 30), "x", TextFormat("%2.2f", x), &x, 0.0f, (float)GetScreenWidth());
    //GuiSlider(rect(30, 460, 200, 30), "y", TextFormat("%2.2f", y), &y, 0.0f, (float)GetScreenWidth());
    //GuiSlider(rect(30, 500, 200, 30), "width", TextFormat("%2.2f", width), &width, 0.0f, (float)GetScreenWidth());
    //GuiSlider(rect(30, 540, 200, 30), "height", TextFormat("%2.2f", height), &height, 0.0f, (float)GetScreenWidth());

    GuiCheckBox(
        rect(orthographic_mode_checkbox_x, orthographic_mode_checkbox_y, orthographic_mode_checkbox_width, orthographic_mode_checkbox_height),
        "Orthographic",
        (bool*)&g_orthographic_mode);

    GuiSlider(rect(fov_slider_x, fov_slider_y, fov_slider_width, fov_slider_height), "FOV", TextFormat("%2.0f", g_fov), &g_fov, 1.0f, 100.0f);
    GuiSlider(rect(near_z_slider_x, near_z_slider_y, near_z_slider_width, near_z_slider_height), "Near Z", TextFormat("%2.1f", g_near_z), &g_near_z, 1, 99.9f);
    GuiSlider(rect(far_z_slider_x, far_z_slider_y, far_z_slider_width, far_z_slider_height), "Far Z", TextFormat("%2.1f", g_far_z), &g_far_z, 100.0f, 10000.0f);

    GuiSlider(rect(x_slider_x, x_slider_y, x_slider_width, x_slider_height), "X", TextFormat("%2.1f", g_vec3.x), &g_vec3.x, -500.0f, 500.0f);
    GuiSlider(rect(y_slider_x, y_slider_y, y_slider_width, y_slider_height), "Y", TextFormat("%2.1f", g_vec3.y), &g_vec3.y, -500.0f, 500.0f);
    GuiSlider(rect(z_slider_x, z_slider_y, z_slider_width, z_slider_height), "Z", TextFormat("%2.1f", g_vec3.z), &g_vec3.z, -500.0f, 500.0f);
}

static void draw_info()
{
    const uint32_t width = raylib_viewport_width();
    const uint32_t height = raylib_viewport_height();
    DrawFPS(2, GetScreenHeight() - 20);

    Vector2 position = {2.0f, 2.0f};
    DrawTextEx(font, TextFormat("Viewport <%dpx, %dpx>", width, height), position, font_size, font_spacing, font_color);
}