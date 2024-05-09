#include "window.h"

#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "styles/dark/style_dark.h"

#include <assert.h>

static Font font = { 0 };
static Image image_buffer;
static Texture2D texture2d;
static const float font_size = 18.0f;
static const float font_spacing = 5.0f;
static const Color font_color = {0, 228, 48, 255};
static bool collapse_control_window = false;
static Color light_color = { 255, 0, 0, 255 };

#define BG_COLOR CLITERAL(Color) { 50, 49, 64, 255 }
#define TRANSPARENT CLITERAL(Color) {0, 0, 0, 0}

// GUI design variables for positioning and sizing components
float gui_component_x = 0.0f, 
    gui_component_y = 0.0f, 
    gui_component_width = 50.0f, 
    gui_component_height = 20.0f;

const float control_window_screen_width_percentage = 0.27f;
float control_window_height;
float control_window_width;

static Color scalar_x_color(const float s, const Color color)
{
    return (Color) {
        s * color.r,
        s * color.g,
        s * color.b,
        s * color.a
    };
}

static Color color_add_color(const Color a, const Color b)
{
    return (Color) {
        a.r + b.r,
        a.g + b.g,
        a.b + b.b,
        a.a + b.a
    };
}

static Rectangle rect(const float x, const float y, const float width, const float height)
{
    return (Rectangle) { x, y, width, height };
}

static void draw_controls();
static void post_processing();
static void smooth_rendered_image();

uint32_t raylib_viewport_width()
{
    return (uint32_t)((1.0f - (float)(!collapse_control_window) * control_window_screen_width_percentage) * GetScreenWidth());
}

uint32_t raylib_viewport_height()
{
    return (uint32_t)GetScreenHeight();
}

Vector2 raylib_get_normalized_coordinate(const Vector2 position)
{
    const float viewport_width = (float)raylib_viewport_width();
    const float viewport_height = (float)raylib_viewport_height();
    const float nx = Clamp(Lerp(-1.0f, 1.0f, position.x / viewport_width), -1.0f, 1.0f);
    const float ny = Clamp(Lerp(1.0f, -1.0f, position.y / viewport_width), -1.0f, 1.0f);
    return (Vector2) { nx, ny };
}

Vector2 raylib_get_screen_coordinates(const Vector2 normalized_coordinates)
{
    const float viewport_width = (float)raylib_viewport_width();
    const float viewport_height = (float)raylib_viewport_height();
    const float x = roundf(Clamp(Lerp(0.0f, viewport_width, (1.0f + normalized_coordinates.x) * 0.5f), 0.0f, viewport_width));
    const float y = roundf(Clamp(Lerp(viewport_height, 0.0f, (1.0f + normalized_coordinates.y) * 0.5f), 0.0f, viewport_height));
    return (Vector2) { x, y };
}

void raylib_show(const char* title, const uint32_t screen_width, const uint32_t screen_height)
{
    InitWindow(screen_width, screen_height, title);

    // Load global data (assets that must be available in all screens, i.e. font)
    font = LoadFont("resources/mecha.png");
    GuiLoadStyleDark();

    control_window_height = 1.0f * GetScreenHeight();
    control_window_width = control_window_screen_width_percentage * GetScreenWidth();

    image_buffer = GenImageWhiteNoise((int)raylib_viewport_width(), (int)raylib_viewport_height(), 1.0f);
    texture2d = LoadTextureFromImage(image_buffer);
}

void raylib_on_update(Window* window, const float ts)
{
    (void)ts;
    window->is_open = (int)!WindowShouldClose();

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

void raylib_end_draw(Window* window)
{
    //post_processing();

    // Drawing overlays last so viewport window doesn't render graphics overtop of it
    draw_controls(window);

    EndDrawing();
}

void raylib_render()
{
    //Texture2D texture = LoadTextureFromImage(image_buffer);
    //post_processing();
    //DrawTexture(texture2d, 0, 0, TRANSPARENT);
    //UnloadTexture(texture);
}

void raylib_draw_pixel(const Vec2 normalized_coordinate, const MyColor color)
{
    const float x = normalized_coordinate.x;
    const float y = normalized_coordinate.y;
    const Vector2 viewport_space = raylib_get_screen_coordinates((Vector2) { x, y });
    const Color c = {color.red, color.green, color.blue, color.alpha};

    // drawing extra pixels to fill in gaps
    //DrawPixel(viewport_space_x - 1, viewport_space_y - 1, c);
    //DrawPixel(viewport_space_x, viewport_space_y - 1, c);
    //DrawPixel(viewport_space_x + 1, viewport_space_y - 1, c);

    //DrawPixel(viewport_space_x - 1, viewport_space_y, c);
    DrawPixel(viewport_space.x, viewport_space.y, c);
    //DrawPixel(viewport_space_x + 1, viewport_space_y, c);

    //DrawPixel(viewport_space_x - 1, viewport_space_y + 1, c);
    //DrawPixel(viewport_space_x, viewport_space_y + 1, c);
    //DrawPixel(viewport_space_x + 1, viewport_space_y + 1, c);

    //ImageDrawPixel(&image_buffer, viewport_space_x - 1, viewport_space_y - 1, c);
    //ImageDrawPixel(&image_buffer, viewport_space_x,     viewport_space_y - 1, c);
    //ImageDrawPixel(&image_buffer, viewport_space_x + 1, viewport_space_y - 1, c);
    //
    //ImageDrawPixel(&image_buffer, viewport_space_x - 1, viewport_space_y, c);
    //ImageDrawPixel(&image_buffer, viewport_space_x, viewport_space_y, c);
    //ImageDrawPixel(&image_buffer, viewport_space_x + 1, viewport_space_y, c);
    //
    //ImageDrawPixel(&image_buffer, viewport_space_x - 1, viewport_space_y + 1, c);
    //ImageDrawPixel(&image_buffer, viewport_space_x,     viewport_space_y + 1, c);
    //ImageDrawPixel(&image_buffer, viewport_space_x + 1, viewport_space_y + 1, c);
}

void post_processing()
{
    smooth_rendered_image();
}

void smooth_rendered_image()
{
    const uint32_t viewport_width = raylib_viewport_width();
    const uint32_t viewport_height = raylib_viewport_height();
    for (uint32_t y = 0; y < viewport_height; ++y)
    {
        for (uint32_t x = 0; x < viewport_width; ++x)
        {
            const float zero_value = 0.0f;
            const float corner_value = 0.5f;
            const float adjacent_value = 0.5f;
            const float target_value = 0.5f;

            const int x_minus_1 = (int)x - 1;
            const int y_minus_1 = (int)y - 1;
            const int x_plus_1 = (int)x + 1;
            const int y_plus_1 = (int)y + 1;

            /* Average the 3x3 area of pixel colors where e is the target pixel
                 x-1  x  x+1
            y-1 | a | b | c |
            y   | d | e | f |
            y+1 | g | h | i |
            */
            const float a = x_minus_1 >= 0 && y_minus_1 >= 0 ? corner_value : zero_value;
            const float b = y_minus_1 >= 0 ? adjacent_value : zero_value;
            const float c = x_plus_1 < viewport_width && y_minus_1 >= 0 ? corner_value : zero_value;
            const float d = x_minus_1 >= 0 ? adjacent_value : zero_value;
            const float e = target_value;
            const float f = x_plus_1 < viewport_width ? adjacent_value : zero_value;
            const float g = x_minus_1 >= 0 && y_plus_1 < viewport_height ? corner_value : zero_value;
            const float h = y_plus_1 < viewport_height ? adjacent_value : zero_value;
            const float i = x_plus_1 < viewport_width && y_plus_1 < viewport_height ? corner_value : zero_value;

            const Color color_a = a != zero_value ? GetImageColor(image_buffer, x_minus_1, y_minus_1) : TRANSPARENT;
            const Color color_b = b != zero_value ? GetImageColor(image_buffer, x, y_minus_1) : TRANSPARENT;
            const Color color_c = c != zero_value ? GetImageColor(image_buffer, x_plus_1, y_minus_1) : TRANSPARENT;
            const Color color_d = d != zero_value ? GetImageColor(image_buffer, x_minus_1, y) : TRANSPARENT;
            const Color color_e = GetImageColor(image_buffer, (int)x, (int)y);
            const Color color_f = f != zero_value ? GetImageColor(image_buffer, x_plus_1, y) : TRANSPARENT;
            const Color color_g = g != zero_value ? GetImageColor(image_buffer, x_minus_1, y_plus_1) : TRANSPARENT;
            const Color color_h = h != zero_value ? GetImageColor(image_buffer, x, y_plus_1) : TRANSPARENT;
            const Color color_i = i != zero_value ? GetImageColor(image_buffer, x_plus_1, y_plus_1) : TRANSPARENT;

            Color color_final = scalar_x_color(a, color_a);
            color_final = color_add_color(color_final, scalar_x_color(b, color_b));
            color_final = color_add_color(color_final, scalar_x_color(c, color_c));
            color_final = color_add_color(color_final, scalar_x_color(d, color_d));
            color_final = color_add_color(color_final, scalar_x_color(e, color_e));
            color_final = color_add_color(color_final, scalar_x_color(f, color_f));
            color_final = color_add_color(color_final, scalar_x_color(g, color_g));
            color_final = color_add_color(color_final, scalar_x_color(h, color_h));
            color_final = color_add_color(color_final, scalar_x_color(i, color_i));

            DrawPixel((int)x, (int)y, color_final);
        }
    }
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
    UnloadImage(image_buffer);
    UnloadTexture(texture2d);

    // Unload global data loaded
    UnloadFont(font);

    // Close window and OpenGL context
    CloseWindow();
}

static void draw_controls(Window* window)
{
    if (collapse_control_window)
    {
        collapse_control_window ^= GuiWindowBox(rect(GetScreenWidth() - 10.0f, 0.0f, 10.0f, 1.0f * GetScreenHeight()), "");
        return;
    }

    const uint32_t viewport_width = raylib_viewport_width();
    const uint32_t viewport_height = raylib_viewport_height();
    const float control_window_x = GetScreenWidth() * (1.0f - control_window_screen_width_percentage);
    const float control_window_y = 0.0f;

    const float fov_slider_height = 22.0f;
    const float fov_slider_width = 113.0f;
    const float fov_slider_x = 633.0f;
    const float fov_slider_y = 39.0f;

    const float near_z_slider_height = 22.0f;
    const float near_z_slider_width = 113.0f;
    const float near_z_slider_x = 633.0f;
    const float near_z_slider_y = 66.0f;

    const float far_z_slider_height = 22.0f;
    const float far_z_slider_width = 113.0f;
    const float far_z_slider_x = 633.0f;
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
    const float orthographic_mode_checkbox_x = 602.0f;
    const float orthographic_mode_checkbox_y = 513.0f;

    const float light_color_picker_height = 175.0f;
    const float light_color_picker_width = 175.0f;
    const float light_color_picker_x = 593.0f;
    const float light_color_picker_y = 243.0f;

    const float light_intensity_slider_height = 22.0f;
    const float light_intensity_slider_width = 50.0f;
    const float light_intensity_slider_x = 687.0f;
    const float light_intensity_slider_y = 435.0f;

    const float light_color_label_height = 22.0f;
    const float light_color_label_width = 78.0f;
    const float light_color_label_x = 595.0f;
    const float light_color_label_y = 221.0f;

    const float status_bar_height = 26.0f;
    const float status_bar_width = 800.0f;
    const float status_bar_x = 0.0f;
    const float status_bar_y = 575.0f;

    const float toggle_active_height = 22.0f;
    const float toggle_active_width = 182.0f;
    const float toggle_active_x = 602.0f;
    const float toggle_active_y = 543.0f;

    collapse_control_window = GuiWindowBox(rect(control_window_x, control_window_y, control_window_width, control_window_height), "Controls");

#if 0
    // GUI Design controls for positioning and sizing components
    GuiSlider(rect(60, 420, 200, 30), "x", TextFormat("%2.2f", gui_component_x), &gui_component_x, 0.0f, (float)GetScreenWidth());
    GuiSlider(rect(60, 460, 200, 30), "y", TextFormat("%2.2f", gui_component_y), &gui_component_y, 0.0f, (float)GetScreenWidth());
    GuiSlider(rect(60, 500, 200, 30), "width", TextFormat("%2.2f", gui_component_width), &gui_component_width, 0.0f, (float)GetScreenWidth());
    GuiSlider(rect(60, 540, 200, 30), "height", TextFormat("%2.2f", gui_component_height), &gui_component_height, 0.0f, (float)GetScreenWidth());
    const Rectangle gui_component_rect = rect(gui_component_x, gui_component_y, gui_component_width, gui_component_height);
#endif

    GuiCheckBox(
        rect(orthographic_mode_checkbox_x, orthographic_mode_checkbox_y, orthographic_mode_checkbox_width, orthographic_mode_checkbox_height),
        "Orthographic Camera",
        (bool*)&window->IsOrthographic);

    GuiSlider(
        rect(fov_slider_x, fov_slider_y, fov_slider_width, fov_slider_height), 
        "FOV", 
        TextFormat("%2.0f", window->FOV), 
        &window->FOV, 1.0f, 100.0f);
    GuiSlider(
        rect(near_z_slider_x, near_z_slider_y, near_z_slider_width, near_z_slider_height), 
        "Near Z", 
        TextFormat("%2.1f", window->NearZ), 
        &window->NearZ, 1, 99.9f);
    GuiSlider(
        rect(far_z_slider_x, far_z_slider_y, far_z_slider_width, far_z_slider_height), 
        "Far Z", 
        TextFormat("%2.1f", window->FarZ), 
        &window->FarZ, 100.0f, 10000.0f);

    //GuiSlider(rect(x_slider_x, x_slider_y, x_slider_width, x_slider_height), "X", TextFormat("%2.1f", g_vec3.x), &g_vec3.x, -500.0f, 500.0f);
    //GuiSlider(rect(y_slider_x, y_slider_y, y_slider_width, y_slider_height), "Y", TextFormat("%2.1f", g_vec3.y), &g_vec3.y, -500.0f, 500.0f);
    //GuiSlider(rect(z_slider_x, z_slider_y, z_slider_width, z_slider_height), "Z", TextFormat("%2.1f", g_vec3.z), &g_vec3.z, -500.0f, 500.0f);

    GuiLabel(rect(light_color_label_x, light_color_label_y, light_color_label_width, light_color_label_height), "Light Color");
    GuiColorPicker(rect(light_color_picker_x, light_color_picker_y, light_color_picker_width, light_color_picker_height), "Light Color", &light_color);
    window->point_lights[0].Color.red = light_color.r;
    window->point_lights[0].Color.green = light_color.g;
    window->point_lights[0].Color.blue = light_color.b;
    window->point_lights[0].Color.alpha = light_color.a;

    GuiSlider(
        rect(light_intensity_slider_x, light_intensity_slider_y, light_intensity_slider_width, light_intensity_slider_height),
        "Light Intesity",
        TextFormat("%2.1f", window->point_lights[0].Intensity),
        &window->point_lights[0].Intensity,
        0.0f, 10000.0f);

    GuiToggleSlider(rect(toggle_active_x, toggle_active_y, toggle_active_width, toggle_active_height), "Light;Camera", &window->is_camera_active);

    GuiSetStyle(STATUSBAR, TEXT_ALIGNMENT, TEXT_ALIGN_RIGHT);
    const Vector2 mouse_position = GetMousePosition();
    const Vector2 ndc = raylib_get_normalized_coordinate(mouse_position);
    GuiStatusBar(
        rect(status_bar_x, status_bar_y, status_bar_width, status_bar_height), 
        TextFormat("Viewport: %d x %d px     FPS: %d     Coordinate: (%2.2f, %2.2f) (%d, %d)", 
            (int)viewport_width, (int)viewport_height, GetFPS(), ndc.x, ndc.y, (int)mouse_position.x, (int)mouse_position.y));
}
