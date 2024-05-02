#include "window.h"
#include "mymath.h"

#include "raylib_impl.h"

#include <math.h>
#include <stdio.h>

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

Window* show(const char* title, const uint32_t viewport_width, const uint32_t viewport_height)
{
    Window* window = create_window_impl();
    window->title = title;
    window->viewport_width = viewport_width;
    window->viewport_height = viewport_height;
    window->aspect_ratio = (float)viewport_width / (float)viewport_height;
    window->is_open = 1;
    window->show(title, viewport_width, viewport_height);
    window->view_matrix = perspective_matrix(window->aspect_ratio, 40.0f, 1.0f, 100.0f);
    return window;
}

void update(Window* window, const float ts)
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

void draw_pixel(Window* window, const Vec3 p1, const MyColor color)
{
    Vec3 p2 = mat4_x_vec3(window->view_matrix, p1);
    if (p2.z == 0.0f)
    {
        printf("z is zero");
        return;
    }

    const Vec2 device_coordinate = vec2(p2.x / p2.z, p2.y / p2.z);
    window->draw_pixel(device_coordinate, color);
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
            const float width_percentage = (float)j / window->viewport_width;
            const float height_percentage = (float)i / window->viewport_height;
            const float normalized_x = 2.0f * width_percentage - 1.0f;
            const float normalized_y = 1.0f - 2.0f * height_percentage;
            const Vec2 device_coordinate = vec2(normalized_x, normalized_y);
            const Vec3 p0 = mat4_x_vec3(window->view_matrix, triangle.p0);
            const Vec3 p1 = mat4_x_vec3(window->view_matrix, triangle.p1);
            const Vec3 p2 = mat4_x_vec3(window->view_matrix, triangle.p2);

            if (p0.z * p1.z * p2.z == 0.0f)
            {
                printf("z is zero");
                return;
            }

            const Vec2 p0_ = vec2(p0.x / p0.z, p0.y / p0.z);
            const Vec2 p1_ = vec2(p1.x / p1.z, p1.y / p1.z);
            const Vec2 p2_ = vec2(p2.x / p2.z, p2.y / p2.z);
            if (is_inside_triangle(device_coordinate, p0_, p1_, p2_))
                window->draw_pixel(device_coordinate, triangle.color);
        }
    }
}
