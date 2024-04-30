#include "window.h"

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
    window->is_open = 1;
    window->show(title, viewport_width, viewport_height);
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

float determinant(const Vec2 p0, const Vec2 p1)
{
    return p0.x * p1.y - p1.x * p0.y;
}

float magnitude(const Vec2 p)
{
    return sqrtf(p.x * p.x + p.y * p.y);
}

Vec2 difference(const Vec2 a, const Vec2 b)
{
    return vec2(a.x - b.x, a.y - b.y);
}

float lerpf(const float a, const float b, const float t)
{
    return (1.0f - t) * a + t * b;
}

MyColor lerp_color(const MyColor a, const MyColor b, const float t)
{
    float red = lerpf(a.red, b.red, t);
    float green = lerpf(a.green, b.green, t);
    float blue = lerpf(a.blue, b.blue, t);
    float alpha = lerpf(a.alpha, b.alpha, t);
    return color((uint8_t)red % 256, (uint8_t)green % 256, (uint8_t)blue % 256, (uint8_t)alpha % 256);
}

Vec2 vec2(const float x, const float y)
{
    return (Vec2) { x, y };
}

MyColor color(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha)
{
    return (MyColor) { red, green, blue, alpha };
}

Triangle triangle(const Vec2 p0, const Vec2 p1, const Vec2 p2, const MyColor color)
{
    Triangle t;
    t.p0 = p0;
    t.p1 = p1;
    t.p2 = p2;
    t.color = color;
    return t;
}

static int is_inside_triangle(const Vec2 p, const Triangle triangle)
{
    Vec2 p0p1 = difference(triangle.p1, triangle.p0);
    Vec2 p1p2 = difference(triangle.p2, triangle.p1);
    Vec2 p2p0 = difference(triangle.p0, triangle.p2);
    Vec2 p0p = difference(p, triangle.p0);
    Vec2 p1p = difference(p, triangle.p1);
    Vec2 p2p = difference(p, triangle.p2);

    float a = determinant(p0p, p0p1);
    float b = determinant(p1p, p1p2);
    float c = determinant(p2p, p2p0);

    return a >= 0 && b >= 0 && c >= 0;
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

            if (is_inside_triangle(p, triangle))
                window->draw_pixel(nx, ny, triangle.color);
        }
    }
}
