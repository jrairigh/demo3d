#pragma once

#include "types.h"

#include <stdint.h>
#include <stdlib.h>

float g_fov;
float g_near_z;
float g_far_z;
bool g_orthographic_mode;
Vec3 g_vec3;

typedef struct Window
{
    const char* title;
    bool is_open;
    uint32_t screen_width, screen_height;
    Mat4 view_matrix;
    float* z_buffer;
    void (*pre_render)();
    void (*render)();
    void (*post_render)();
    void (*draw_pixel)(const Vec2 device_coordinate, const MyColor color);
    void (*draw_line)(const Vec2 start, const Vec2 end, const MyColor color);
    void (*draw_overlay_text)(const char* text, const Vec2 position, const MyColor color);
    void (*close_window)();
    void (*on_update)(struct Window* window, const float ts);
    void (*show)(const char* title, const uint32_t screen_width, const uint32_t screen_height);
    uint32_t (*get_viewport_width)();
    uint32_t (*get_viewport_height)();
} Window;

Window* show(const char* title, const uint32_t viewport_width, const uint32_t viewport_height);
void update(Window* window);
void render(Window* window);
void close_window(Window** window);

void draw_pixel(Window* window, const Vec3 p1, const MyColor color);
void draw_line(Window* window, const Vec3 start, const Vec3 end, const MyColor color);
void draw_triangle(Window* window, const Triangle triangle);
void draw_triangles(Window* window, const Triangles triangles, const uint32_t triangle_count);
void draw_overlay_text(Window* window, const char* text, const Vec2 position, const MyColor color);
