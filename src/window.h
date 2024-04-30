#pragma once

#include <stdint.h>
#include <stdlib.h>

typedef struct _tagColor
{
    uint8_t red, green, blue, alpha;
} MyColor;

typedef struct _tagVec2
{
    float x, y;
} Vec2;

typedef struct _tagTriangle
{
    Vec2 p0, p1, p2;
    MyColor color;
} Triangle;

typedef Triangle* Triangles;

typedef struct _tagWindow
{
    const char* title;
    int is_open;
    uint32_t viewport_width, viewport_height;
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

Triangle triangle(const Vec2 p0, const Vec2 p1, const Vec2 p2, const MyColor color);
MyColor color(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha);
Vec2 vec2(const float x, const float y);
float lerpf(const float a, const float b, const float t);
MyColor lerp_color(const MyColor a, const MyColor b, const float t);
Vec2 difference(const Vec2 a, const Vec2 b);
float determinant(const Vec2 p0, const Vec2 p1);
float magnitude(const Vec2 p);

void draw_pixel(Window* window, const Vec2 p1, const MyColor color);
void draw_triangle(Window* window, const Triangle triangle);
void draw_triangles(Window* window, const Triangles triangles, const uint32_t triangle_count);
