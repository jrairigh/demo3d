#pragma once

#include "types.h"

#include "camera.h"
#include "point_light.h"

#include <stdint.h>
#include <stdlib.h>

Vec3 g_vec3;

typedef struct Window
{
    const char* title;
    bool is_open;
    bool is_camera_active;
    bool IsOrthographic;
    bool wasd_key_state[4];
    uint32_t screen_width, screen_height;
    MyCamera camera;
    PointLight point_lights[2];
    uint32_t number_of_lights;
    float FOV;
    float NearZ;
    float FarZ;
    float CameraSpeed;
    float* z_buffer;
    void (*pre_render)();
    void (*render)();
    void (*post_render)(struct Window* window);
    void (*draw_pixel)(const Vec2 device_coordinate, const MyColor color);
    void (*draw_line)(const Vec2 start, const Vec2 end, const MyColor color);
    void (*draw_light_widget)(const Vec2 center, const float radius, const MyColor color);
    void (*draw_overlay_text)(const char* text, const Vec2 position, const MyColor color);
    void (*close_window)();
    void (*on_update)(struct Window* window, const float ts);
    void (*show)(const char* title, const uint32_t screen_width, const uint32_t screen_height);
    uint32_t (*get_viewport_width)();
    uint32_t (*get_viewport_height)();
    float (*get_frame_elapsed_seconds)();
} Window;

Window* show(const char* title, const uint32_t viewport_width, const uint32_t viewport_height);
void update(Window* window);
void render(Window* window);
void close_window(Window** window);

void draw_pixel(Window* window, const Vec3 p1, const MyColor color);
void draw_line(Window* window, const Vec3 start, const Vec3 end, const MyColor color);
void draw_light_widget(Window* window, const Vec3 center, const float radius, const MyColor color);
void draw_triangle(Window* window, const Triangle triangle);
void draw_triangles(Window* window, const Triangles triangles, const uint32_t triangle_count);
void draw_overlay_text(Window* window, const char* text, const Vec2 position, const MyColor color);
void draw_wireframe_box(Window* window, const Vec3 scale, const float angle, const Vec3 position, const MyColor color);
void draw_grid(Window* window, const float y, const float cell_width, const float cell_height, const uint32_t rows, const uint32_t columns, const MyColor color);
void draw_cube(Window* window, const Vec3 position, const Vec3 scale, const MyColor color);

void add_light_source(Window* window, const PointLight light);
