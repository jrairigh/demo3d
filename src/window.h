#pragma once

#include "types.h"

#include <stdint.h>
#include <stdlib.h>

typedef struct _tagWindow
{
    const char* title;
    int is_open;
    uint32_t viewport_width, viewport_height;
    float aspect_ratio;
    Mat4 view_matrix;
    void (*pre_render)();
    void (*post_render)();
    void (*draw_pixel)(float x, float y, MyColor color);
    void (*close_window)();
    void (*on_update)(struct _tagWindow* window, float ts);
    void (*show)(const char* title, uint32_t viewport_width, uint32_t viewport_height);
} Window;

Window* show(const char* title, uint32_t viewport_width, uint32_t viewport_height);
void update(Window* window, float ts);
void render(Window* window);
void close_window(Window** window);

void draw_pixel(Window* window, const Vec2 p1, const MyColor color);
void draw_triangle(Window* window, const Triangle triangle);
void draw_triangles(Window* window, const Triangles triangles, const uint32_t triangle_count);
