#pragma once

#include <stdint.h>
#include <stdlib.h>

typedef struct _tagColor
{
    uint8_t red, green, blue, alpha;
} MyColor;

typedef struct _tagWindow
{
    const char* title;
    int is_open;
    uint32_t viewport_width, viewport_height;
    void (*draw_pixel)(float x, float y, MyColor color);
    void (*close_window)();
    void (*on_update)(struct _tagWindow* window, float ts);
    void (*show)(const char* title, uint32_t viewport_width, uint32_t viewport_height);
} Window;

Window* show(const char* title, uint32_t viewport_width, uint32_t viewport_height);
void update(Window* window, float ts);
void render(const Window* window);
void close_window(Window** window);
