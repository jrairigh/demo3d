#include "window.h"
#include "mymath.h"

#include "raylib_impl.h"

#include <math.h>

Window* create_window_impl()
{
    Window* window = (Window*)malloc(sizeof(Window));
    window->pre_render = raylib_begin_draw;
    window->post_render = raylib_end_draw;
    window->on_update = raylib_on_update;
    window->draw_pixel = raylib_draw_pixel;
    window->close_window = raylib_close_window;
    window->show = raylib_show;
    return window;
}

Window* show(const char* title, uint32_t viewport_width, uint32_t viewport_height)
{
    Window* window = create_window_impl();
    window->title = title;
    window->viewport_width = viewport_width;
    window->viewport_height = viewport_height;
    window->aspect_ratio = (float)viewport_width / (float)viewport_height;
    window->is_open = 1;
    window->show(title, viewport_width, viewport_height);
    window->view_matrix = perspective_matrix(window->aspect_ratio, 40.0f, 1.0f, 10.0f);
    return window;
}

void update(Window* window, float ts)
{
    while (window->is_open > 0)
    {
        window->on_update(window, ts);
        window->pre_render();
        render(window);
        window->post_render();
    }
}

void close_window(Window** window_ptr)
{
    Window* w = *window_ptr;
    w->close_window();
    free(w);
    *window_ptr = NULL;
}

void draw_pixel(Window* window, const Vec2 p1, const MyColor color)
{
    window->draw_pixel(p1.x, p1.y, color);
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
    for (uint32_t i = 0; i < window->viewport_height; ++i)
    {
        for (uint32_t j = 0; j < window->viewport_width; ++j)
        {
            const float nx = j * (2.0f / window->viewport_width) - 1.0f;
            const float ny = 1.0f - i * (2.0f / window->viewport_height);
            const Vec2 p = vec2(nx, ny);
            const Vec3 p0 = mat4_x_vec3(window->view_matrix, triangle.p0);
            const Vec3 p1 = mat4_x_vec3(window->view_matrix, triangle.p1);
            const Vec3 p2 = mat4_x_vec3(window->view_matrix, triangle.p2);

            if (is_inside_triangle(p, vec2(p0.x / p0.z, p0.y / p0.z), vec2(p1.x / p1.z, p1.y / p1.z), vec2(p2.x / p2.z, p2.y / p2.z)))
                window->draw_pixel(nx, ny, triangle.color);
        }
    }
}
