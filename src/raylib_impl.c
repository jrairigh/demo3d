#include "window.h"

#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "styles/dark/style_dark.h"

#include <assert.h>

static const float font_size = 14.0f;
static const float font_spacing = 5.0f;
static const Color font_color = {0, 228, 48, 255};
static bool collapse_control_window = false;
static Color light_color = { 255, 0, 0, 255 };
static int mesh_selection_index = 0;
static bool refresh_meshes = true;

typedef enum MeshId
{
    Poly,
    Plane,
    Cube,
    Sphere,
    HemiSphere,
    Cylinder,
    Cone,
    Torus,
    Knot
} MeshId;

typedef struct MeshSelection
{
    MeshId Id;
    const char* FriendlyName;
    bool IsLoaded;
    Mesh MeshData;
} MeshSelection;

MeshSelection meshes[9];

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

static void draw_controls();
static void draw_fixed_fps_control();
static void post_processing();
//static void smooth_rendered_image();
static void unload_meshes();
static void generate_meshes(const Vec4 controls);

static MeshSelection mesh_selection(const MeshId id, const char* friendly_name, const Mesh mesh)
{
    return (MeshSelection) { id, friendly_name, true, mesh };
}

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
    GuiLoadStyleDark();

    control_window_height = 1.0f * GetScreenHeight();
    control_window_width = control_window_screen_width_percentage * GetScreenWidth();
}

void raylib_on_update(Window* window, const float ts)
{
    (void)ts;
    window->is_open = (int)!WindowShouldClose();

    window->wasd_key_state[0] = IsKeyDown(KEY_W);
    window->wasd_key_state[1] = IsKeyDown(KEY_A);
    window->wasd_key_state[2] = IsKeyDown(KEY_S);
    window->wasd_key_state[3] = IsKeyDown(KEY_D);

    generate_meshes(window->Controls);
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
}

float raylib_get_frame_elapsed_seconds()
{
    return GetFrameTime();
}

void raylib_draw_pixel(const Vec2 normalized_coordinate, const MyColor color)
{
    const Vector2 viewport_space = raylib_get_screen_coordinates((Vector2) { normalized_coordinate.x, normalized_coordinate.y});
    DrawPixelV(viewport_space, (Color) { color.red, color.green, color.blue, color.alpha });
}

void post_processing()
{
    smooth_rendered_image();
}

/*
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

            //Average the 3x3 area of pixel colors where e is the target pixel
            //     x-1  x  x+1
            //y-1 | a | b | c |
            //y   | d | e | f |
            //y+1 | g | h | i |
            //
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
*/

void raylib_draw_line(const Vec2 start, const Vec2 end, const MyColor color)
{
    const Vector2 p0 = raylib_get_screen_coordinates((Vector2) { start.x, start.y });
    const Vector2 p1 = raylib_get_screen_coordinates((Vector2) { end.x, end.y });

    DrawLineV(p0, p1, (Color) { color.red, color.green, color.blue, color.alpha });
}

void raylib_light_widget(const Vec2 center, const float radius, const MyColor color)
{
    const Color circle_color = { color.red, color.green, color.blue, color.alpha };
    const Vector2 circle_center = { center.x, center.y };
    DrawCircleLinesV(raylib_get_screen_coordinates(circle_center), radius, circle_color);
}

void raylib_draw_overlay_text(const char* text, const Vec2 position, const MyColor color)
{
    DrawText(text, position.x, position.y, font_size, (Color) { color.red, color.green, color.blue, color.alpha });
}

float* raylib_get_mesh_triangle_points(const int mesh_id)
{
    (void)mesh_id;
    return meshes[mesh_selection_index].MeshData.vertices;
}

uint32_t raylib_get_mesh_vertex_count(const int mesh_id)
{
    (void)mesh_id;
    return (uint32_t)meshes[mesh_selection_index].MeshData.vertexCount;
}

uint32_t raylib_get_mesh_triangle_count(const int mesh_id)
{
    (void)mesh_id;
    return (uint32_t)meshes[mesh_selection_index].MeshData.triangleCount;
}

void raylib_close_window()
{
    unload_meshes();

    // Close window and OpenGL context
    CloseWindow();
}

void draw_controls(Window* window)
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

    const float speed_slider_height = 22.0f;
    const float speed_slider_width = 113.0f;
    const float speed_slider_x = 633.0f;
    const float speed_slider_y = 120.0f;

    const float mesh_dropdown_height = 22.0f;
    const float mesh_dropdown_width = 80.0f;
    const float mesh_dropdown_x = 595.0f;
    const float mesh_dropdown_y = 147.0f;

    const float a_slider_height = 22.0f;
    const float a_slider_width = 90.0f;
    const float a_slider_x = 700.0f;
    const float a_slider_y = 147.0f;

    const float b_slider_height = 22.0f;
    const float b_slider_width = 90.0f;
    const float b_slider_x = 700.0f;
    const float b_slider_y = 174.0f;

    const float c_slider_height = 22.0f;
    const float c_slider_width = 90.0f;
    const float c_slider_x = 700.0f;
    const float c_slider_y = 201.0f;

    const float d_slider_height = 22.0f;
    const float d_slider_width = 90.0f;
    const float d_slider_x = 700.0f;
    const float d_slider_y = 228.0f;

    const float orthographic_mode_checkbox_height = 20.0f;
    const float orthographic_mode_checkbox_width = 20.0f;
    const float orthographic_mode_checkbox_x = 602.0f;
    const float orthographic_mode_checkbox_y = 513.0f;

    const float light_color_picker_height = 175.0f;
    const float light_color_picker_width = 175.0f;
    const float light_color_picker_x = 593.0f;
    const float light_color_picker_y = 270.0f;

    const float light_intensity_slider_height = 22.0f;
    const float light_intensity_slider_width = 50.0f;
    const float light_intensity_slider_x = 687.0f;
    const float light_intensity_slider_y = 455.0f;

    const float light_color_label_height = 22.0f;
    const float light_color_label_width = 78.0f;
    const float light_color_label_x = 595.0f;
    const float light_color_label_y = 250.0f;

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

    draw_fixed_fps_control();

    GuiSlider(
        rect(fov_slider_x, fov_slider_y, fov_slider_width, fov_slider_height), 
        "FOV", 
        TextFormat("%2.0f", window->FOV), 
        &window->FOV, 1.0f, 100.0f);
    GuiSlider(
        rect(near_z_slider_x, near_z_slider_y, near_z_slider_width, near_z_slider_height), 
        "Near Z", 
        TextFormat("%2.1f", window->NearZ), 
        &window->NearZ, 0.1f, 99.9f);
    GuiSlider(
        rect(far_z_slider_x, far_z_slider_y, far_z_slider_width, far_z_slider_height), 
        "Far Z", 
        TextFormat("%2.1f", window->FarZ), 
        &window->FarZ, 100.0f, 10000.0f);

    GuiSlider(
        rect(speed_slider_x, speed_slider_y, speed_slider_width, speed_slider_height),
        "Speed", 
        TextFormat("%2.1f", window->CameraSpeed), 
        &window->CameraSpeed, 
        1.0f, 500.0f);

    static bool spinner_a_edit_mode = false;
    int control_a_value = window->Controls.x;
    const int clicked_a = GuiSpinner(rect(a_slider_x, a_slider_y, a_slider_width, a_slider_height), "A:", &control_a_value, 1, 500, spinner_a_edit_mode);
    if (clicked_a && !spinner_a_edit_mode)
    {
        spinner_a_edit_mode = true;
    }
    else if (clicked_a)
    {
        spinner_a_edit_mode = false;
    }

    static bool spinner_b_edit_mode = false;
    int control_b_value = window->Controls.y;
    const int clicked_b = GuiSpinner(rect(b_slider_x, b_slider_y, b_slider_width, b_slider_height), "B:", &control_b_value, 1, 500, spinner_b_edit_mode);
    if (clicked_b && !spinner_b_edit_mode)
    {
        spinner_b_edit_mode = true;
    }
    else if (clicked_b)
    {
        spinner_b_edit_mode = false;
    }

    static bool spinner_c_edit_mode = false;
    int control_c_value = window->Controls.z;
    const int clicked_c = GuiSpinner(rect(c_slider_x, c_slider_y, c_slider_width, c_slider_height), "C:", &control_c_value, 1, 500, spinner_c_edit_mode);
    if (clicked_c && !spinner_c_edit_mode)
    {
        spinner_c_edit_mode = true;
    }
    else if (clicked_c)
    {
        spinner_c_edit_mode = false;
    }

    static bool spinner_d_edit_mode = false;
    int control_d_value = window->Controls.w;
    const int clicked_d = GuiSpinner(rect(d_slider_x, d_slider_y, d_slider_width, d_slider_height), "D:", &control_d_value, 1, 500, spinner_d_edit_mode);
    if (clicked_d && !spinner_d_edit_mode)
    {
        spinner_d_edit_mode = true;
    }
    else if (clicked_d)
    {
        spinner_d_edit_mode = false;
    }

    refresh_meshes = control_a_value != window->Controls.x || 
        control_b_value != window->Controls.y || 
        control_c_value != window->Controls.z ||
        control_d_value != window->Controls.w;

    window->Controls.x = control_a_value;
    window->Controls.y = control_b_value;
    window->Controls.z = control_c_value;
    window->Controls.w = control_d_value;

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

    GuiSetStyle(STATUSBAR, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    const Vector2 mouse_position = GetMousePosition();
    const Vector2 ndc = raylib_get_normalized_coordinate(mouse_position);
    GuiStatusBar(
        rect(status_bar_x, status_bar_y, status_bar_width, status_bar_height),
        TextFormat("Viewport: %d x %d   FPS: %d   Mouse: (%4.2f, %-2.2f) (%d, %d)   World: (%2.2f, %2.2f, %2.2f)   Facing: (%2.2f, %2.2f, %2.2f)",
            (int)viewport_width, (int)viewport_height,
            GetFPS(),
            ndc.x, ndc.y, (int)mouse_position.x, (int)mouse_position.y,
            window->camera.Position.x, window->camera.Position.y, window->camera.Position.z,
            window->camera.LookAt.x, window->camera.LookAt.y, window->camera.LookAt.z));

    GuiUnlock();

    static bool dropdown_edit_mode = false;
    if (dropdown_edit_mode)
        GuiLock();

    const bool clicked = GuiDropdownBox(rect(mesh_dropdown_x, mesh_dropdown_y, mesh_dropdown_width, mesh_dropdown_height),
        "Poly;Plane;Cube;Sphere;HemiSphere;Cylinder;Cone;Torus;Knot",
        &mesh_selection_index, dropdown_edit_mode);
    if (clicked && !dropdown_edit_mode)
    {
        // expanding dropdown to make a new mesh selection
        dropdown_edit_mode = true;
    }
    else if(clicked)
    {
        // mesh selection is made
        dropdown_edit_mode = false;
        refresh_meshes = true;
    }
}

void draw_fixed_fps_control()
{
    const float fixed_fps_checkbox_height = 20.0f;
    const float fixed_fps_checkbox_width = 20.0f;
    const float fixed_fps_checkbox_x = 602.0f;
    const float fixed_fps_checkbox_y = 482.0f;

    static bool is_fixed_fps_state;
    static bool fixed_fps = false;
    GuiCheckBox(
        rect(fixed_fps_checkbox_x, fixed_fps_checkbox_y, fixed_fps_checkbox_width, fixed_fps_checkbox_height),
        "Fixed FPS",
        &is_fixed_fps_state);
    if (is_fixed_fps_state && !fixed_fps)
    {
        fixed_fps = true;
        SetTargetFPS(60);
    }

    if (!is_fixed_fps_state && fixed_fps)
    {
        fixed_fps = false;
        SetTargetFPS(0);
    }
}

static MeshSelection generate_mesh_poly(const Vec4 controls)
{
    if (meshes[Poly].IsLoaded)
        UnloadMesh(meshes[Poly].MeshData);

    const Mesh mesh = GenMeshPoly(controls.x, controls.y);
    return mesh_selection(Poly, "Poly", mesh);
}

static MeshSelection generate_mesh_plane(const Vec4 controls)
{
    if (meshes[Plane].IsLoaded)
        UnloadMesh(meshes[Plane].MeshData);

    const Mesh mesh = GenMeshPlane(controls.x, controls.y, (int)controls.z, (int)controls.w);
    return mesh_selection(Plane, "Plane", mesh);
}

static MeshSelection generate_mesh_cube(const Vec4 controls)
{
    if (meshes[Cube].IsLoaded)
        UnloadMesh(meshes[Cube].MeshData);

    const Mesh mesh = GenMeshCube(controls.x, controls.y, controls.z);
    return mesh_selection(Cube, "Cube", mesh);
}

static MeshSelection generate_mesh_sphere(const Vec4 controls)
{
    if (meshes[Sphere].IsLoaded)
        UnloadMesh(meshes[Sphere].MeshData);

    const Mesh mesh = GenMeshSphere(controls.x, (int)controls.y, (int)controls.z);
    return mesh_selection(Sphere, "Sphere", mesh);
}

static MeshSelection generate_mesh_hemisphere(const Vec4 controls)
{
    if (meshes[HemiSphere].IsLoaded)
        UnloadMesh(meshes[HemiSphere].MeshData);

    const Mesh mesh = GenMeshHemiSphere(controls.x, (int)controls.y, (int)controls.z);
    return mesh_selection(HemiSphere, "Hemisphere", mesh);
}

static MeshSelection generate_mesh_cylinder(const Vec4 controls)
{
    if (meshes[Cylinder].IsLoaded)
        UnloadMesh(meshes[Cylinder].MeshData);

    const Mesh mesh = GenMeshCylinder(controls.x, controls.y, (int)controls.z);
    return mesh_selection(Cylinder, "Cylinder", mesh);
}

static MeshSelection generate_mesh_cone(const Vec4 controls)
{
    if (meshes[Cone].IsLoaded)
        UnloadMesh(meshes[Cone].MeshData);

    const Mesh mesh = GenMeshCone(controls.x, controls.y, (int)controls.z);
    return mesh_selection(Cone, "Cone", mesh);
}

static MeshSelection generate_mesh_torus(const Vec4 controls)
{
    if (meshes[Torus].IsLoaded)
        UnloadMesh(meshes[Torus].MeshData);

    const Mesh mesh = GenMeshTorus(controls.x, controls.y, (int)controls.z, (int)controls.w);
    return mesh_selection(Torus, "Torus", mesh);
}

static MeshSelection generate_mesh_knot(const Vec4 controls)
{
    if (meshes[Knot].IsLoaded)
        UnloadMesh(meshes[Knot].MeshData);

    const Mesh mesh = GenMeshKnot(controls.x, controls.y, (int)controls.z, (int)controls.w);
    return mesh_selection(Knot, "Knot", mesh);
}

static void generate_meshes(const Vec4 controls)
{
    if (!refresh_meshes)
        return;

    refresh_meshes = false;
    switch (mesh_selection_index)
    {
    case Poly:
        meshes[Poly] = generate_mesh_poly(controls);
        break;
    case Plane:
        meshes[Plane] = generate_mesh_plane(controls);
        break;
    case Cube:
        meshes[Cube] = generate_mesh_cube(controls);
        break;
    case Sphere:
        meshes[Sphere] = generate_mesh_sphere(controls);
        break;
    case HemiSphere:
        meshes[HemiSphere] = generate_mesh_hemisphere(controls);
        break;
    case Cylinder:
        meshes[Cylinder] = generate_mesh_cylinder(controls);
        break;
    case Cone:
        meshes[Cone] = generate_mesh_cone(controls);
        break;
    case Torus:
        meshes[Torus] = generate_mesh_torus(controls);
        break;
    case Knot:
        meshes[Knot] = generate_mesh_knot(controls);
        break;
    default:
        break;
    }
}

static void unload_meshes()
{
    MeshSelection* selection = meshes;
    MeshSelection* last = meshes + _countof(meshes);
    for (; selection != last; ++selection)
    {
        if (!selection->IsLoaded)
            continue;

        UnloadMesh(selection->MeshData);
    }
}
