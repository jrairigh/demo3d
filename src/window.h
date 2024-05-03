#pragma once

#include "types.h"

#include <stdint.h>
#include <stdlib.h>

float g_fov;
float g_near_z;
float g_far_z;

typedef struct Window
{
    const char* title;
    bool is_open;
    uint32_t screen_width, screen_height;
    Mat4 view_matrix;
    void (*pre_render)();
    void (*post_render)();
    void (*draw_pixel)(const Vec2 device_coordinate, const MyColor color);
    void (*close_window)();
    void (*on_update)(struct Window* window, const float ts);
    void (*show)(const char* title, const uint32_t screen_width, const uint32_t screen_height);
    float (*get_viewport_width)();
    float (*get_viewport_height)();
} Window;

Window* show(const char* title, const uint32_t viewport_width, const uint32_t viewport_height);
void update(Window* window, const float ts);
void render(Window* window);
void close_window(Window** window);

void draw_pixel(Window* window, const Vec3 p1, const MyColor color);
void draw_triangle(Window* window, const Triangle triangle);
void draw_triangles(Window* window, const Triangles triangles, const uint32_t triangle_count);
