#include "window.h"
#include "mymath.h"

#include "raylib_impl.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

float g_fov = 90.0f;
float g_near_z = 1.0f;
float g_far_z = 999.9f;
bool g_orthographic_mode = 0;
Vec3 g_vec3 = { 1.0f, 1.0f, 1.0f };

Window* create_window_impl()
{
    Window* window = (Window*)malloc(sizeof(Window));
    window->pre_render = raylib_begin_draw;
    window->render = raylib_render;
    window->post_render = raylib_end_draw;
    window->on_update = raylib_on_update;
    window->draw_pixel = raylib_draw_pixel;
    window->draw_line = raylib_draw_line;
    window->draw_overlay_text = raylib_draw_overlay_text;
    window->close_window = raylib_close_window;
    window->show = raylib_show;
    window->get_viewport_width = raylib_viewport_width;
    window->get_viewport_height = raylib_viewport_height;
    return window;
}

static void clear_z_buffer(Window* window)
{
    const uint32_t viewport_width = window->get_viewport_width();
    const uint32_t viewport_height = window->get_viewport_height();

    // TODO need to reallocate z_buffer if viewport changed
    // The canonical view volume z values are between 0 and 1, so choosing an impossible value than will always be overriden by something in view.
    const int n = viewport_width * viewport_height;
    for (int i = 0; i < n; ++i)
        window->z_buffer[i] = 99.0f;
}

Window* show(const char* title, const uint32_t screen_width, const uint32_t screen_height)
{
    Window* window = create_window_impl();
    window->title = title;
    window->screen_width = screen_width;
    window->screen_height = screen_height;
    window->is_open = true;
    window->show(title, screen_width, screen_height);

    const uint32_t viewport_width = window->get_viewport_width();
    const uint32_t viewport_height = window->get_viewport_height();
    window->z_buffer = (float*)malloc(viewport_width * viewport_height * sizeof(float));
    clear_z_buffer(window);
    return window;
}

void update(Window* window)
{
    const float elapsed_time = 0.16667f;
    while (window->is_open)
    {
        const float viewport_width = (float)window->get_viewport_width();
        const float viewport_height = (float)window->get_viewport_height();
        const float aspect_ratio = viewport_width / viewport_height;
        const float x = fabsf(g_vec3.x);
        const float y = fabsf(g_vec3.y);
        const float z = fabsf(g_vec3.z);

        window->view_matrix = g_orthographic_mode 
            ? orthographic_mat4(-x, x, -y, y, -z, z) 
            : perspective_mat4(aspect_ratio, g_fov, g_near_z, g_far_z);

        window->on_update(window, elapsed_time);
        window->pre_render();
        render(window, elapsed_time);
        //window->render();
        window->post_render();
    }
}

void close_window(Window** window_ptr)
{
    Window* w = *window_ptr;
    w->close_window();
    free(w->z_buffer);
    free(w);
    *window_ptr = NULL;
}

void draw_pixel(Window* window, const Vec3 p1, const MyColor color)
{
    const Vec4 p2 = mat4_x_vec4(window->view_matrix, vec4(p1.x, p1.y, p1.z, 1.0f));
    if (p2.w == 0.0f)
    {
        printf("w is zero\n");
        return;
    }

    const Vec4 position = scalar_x_vec4((1.0f / p2.w), p2);
    const float x = position.x;
    const float y = position.y;
    const float z = position.z;

    const bool inside_view = is_within_canonical_view_volume(x, y, z);
    if (!inside_view)
        return;

    window->draw_pixel(vec2(x, y), color);
}

void draw_line(Window* window, const Vec3 start, const Vec3 end, const MyColor color)
{
    Vec4 start_ = vec4(start.x, start.y, start.z, 1.0f);
    Vec4 end_ = vec4(end.x, end.y, end.z, 1.0f);
    Vec4 p0 = mat4_x_vec4(window->view_matrix, start_);
    Vec4 p1 = mat4_x_vec4(window->view_matrix, end_);

    if (p0.w == 0.0f || p1.w == 0.0f)
    {
        printf("w is zero\n");
        return;
    }

    const Vec4 start_position = scalar_x_vec4((1.0f / p0.w), p0);
    const Vec4 end_position   = scalar_x_vec4((1.0f / p1.w), p1);
    const float sx = start_position.x;
    const float sy = start_position.y;
    const float sz = start_position.z;
    const float ex = end_position.x;
    const float ey = end_position.y;
    const float ez = end_position.z;

    // TODO need to handle lines that are partially clipped
    const bool inside_view = is_within_canonical_view_volume(sx, sy, sz) && is_within_canonical_view_volume(ex, ey, ez);
    if(!inside_view)
        return;

    window->draw_line(vec2(sx, sy), vec2(ex, ey), color);
}

void draw_triangles(Window* window, const Triangles triangle, const uint32_t triangle_count)
{
    for (uint32_t i = 0; i < triangle_count; ++i)
    {
        draw_triangle(window, triangle[i]);
    }
}

void draw_triangle(Window* window, const Triangle triangle)
{
    const uint32_t viewport_height = window->get_viewport_height();
    const uint32_t viewport_width = window->get_viewport_width();
    for (uint32_t i = 0; i < viewport_height; ++i)
    {
        for (uint32_t j = 0; j < viewport_width; ++j)
        {
            // convert from screen to normalized device coordinates
            const float width_percentage = (float)j / (float)viewport_width;
            const float height_percentage = (float)i / (float)viewport_height;
            const float normalized_x = 2.0f * width_percentage - 1.0f;
            const float normalized_y = 1.0f - 2.0f * height_percentage;
            const Vec2 device_coordinate = vec2(normalized_x, normalized_y);

            // transform by view matrix
            const Vec4 p0 = mat4_x_vec4(window->view_matrix, vec3_to_vec4(triangle.p0, 1.0f));
            const Vec4 p1 = mat4_x_vec4(window->view_matrix, vec3_to_vec4(triangle.p1, 1.0f));
            const Vec4 p2 = mat4_x_vec4(window->view_matrix, vec3_to_vec4(triangle.p2, 1.0f));

            if (p0.w * p1.w * p2.w == 0.0f)
            {
                printf("w is zero\n");
                continue;
            }

            // scale by w
            const Vec4 p0_ = scalar_x_vec4((1.0f / p0.w), p0);
            const Vec4 p1_ = scalar_x_vec4((1.0f / p1.w), p1);
            const Vec4 p2_ = scalar_x_vec4((1.0f / p2.w), p2);

            // cull
            const bool inside_view = is_within_canonical_view_volume(p0_.x, p0_.y, p0_.z) &&
                is_within_canonical_view_volume(p1_.x, p1_.y, p1_.z) &&
                is_within_canonical_view_volume(p2_.x, p2_.y, p2_.z);
            if (!inside_view)
                continue;

            // test within triangle and not occluded by something in front
            const float z = is_within_triangle(device_coordinate, 
                vec3(p0_.x, p0_.y, p0_.z), 
                vec3(p1_.x, p1_.y, p1_.z), 
                vec3(p2_.x, p2_.y, p2_.z));
            if (z < 0.0f || z > window->z_buffer[i * viewport_width + j])
                continue;

            window->z_buffer[i * viewport_width + j] = z;
            window->draw_pixel(device_coordinate, triangle.color);
        }
    }
}

void draw_overlay_text(Window* window, const char* text, const Vec2 position, const MyColor color)
{
    window->draw_overlay_text(text, position, color);
}

void draw_wireframe_box(Window* window, const float width, const float height, const float depth, const float angle, const Vec3 position, const MyColor color)
{
    const float x = width * 0.5f;
    const float y = height * 0.5f;
    const float z = depth * 0.5f;

    // box corners
    const Vec3 a = vec3(-x, y, -z);
    const Vec3 b = vec3(x, y, -z);
    const Vec3 c = vec3(x, -y, -z);
    const Vec3 d = vec3(-x, -y, -z);
    const Vec3 e = vec3(-x, y, z);
    const Vec3 f = vec3(x, y, z);
    const Vec3 g = vec3(x, -y, z);
    const Vec3 h = vec3(-x, -y, z);

    // rotate corners
    const Mat3 rotate_y_matrix = rotate_y_axis(angle);
    const Vec3 a_ = vec3_add_vec3(mat3_x_vec3(rotate_y_matrix, a), position);
    const Vec3 b_ = vec3_add_vec3(mat3_x_vec3(rotate_y_matrix, b), position);
    const Vec3 c_ = vec3_add_vec3(mat3_x_vec3(rotate_y_matrix, c), position);
    const Vec3 d_ = vec3_add_vec3(mat3_x_vec3(rotate_y_matrix, d), position);
    const Vec3 e_ = vec3_add_vec3(mat3_x_vec3(rotate_y_matrix, e), position);
    const Vec3 f_ = vec3_add_vec3(mat3_x_vec3(rotate_y_matrix, f), position);
    const Vec3 g_ = vec3_add_vec3(mat3_x_vec3(rotate_y_matrix, g), position);
    const Vec3 h_ = vec3_add_vec3(mat3_x_vec3(rotate_y_matrix, h), position);

    // front face
    draw_line(window, a_, b_, color);
    draw_line(window, b_, c_, color);
    draw_line(window, c_, d_, color);
    draw_line(window, d_, a_, color);
    draw_line(window, a_, c_, color);

    // back face
    draw_line(window, e_, f_, color);
    draw_line(window, f_, g_, color);
    draw_line(window, g_, h_, color);
    draw_line(window, h_, e_, color);
    draw_line(window, h_, f_, color);

    // top face
    draw_line(window, a_, e_, color);
    draw_line(window, b_, f_, color);
    draw_line(window, b_, e_, color);

    // bottom face
    draw_line(window, d_, h_, color);
    draw_line(window, c_, g_, color);
    draw_line(window, d_, g_, color);

    // side faces
    draw_line(window, a_, h_, color);
    draw_line(window, c_, f_, color);
}
