#pragma once

#include "window.h"

void raylib_draw_pixel(const Vec2 normalized_coordinate, const MyColor color);
void raylib_draw_line(const Vec2 start, const Vec2 end, const MyColor color);
void raylib_light_widget(const Vec2 center, const float radius, const MyColor color);
void raylib_draw_overlay_text(const char* text, const Vec2 position, const MyColor color);
void raylib_close_window();
void raylib_show(const char* title, const uint32_t screen_width, const uint32_t screen_height);
void raylib_on_update(Window* window, const float ts);
void raylib_begin_draw();
void raylib_render();
void raylib_end_draw(Window* window);
uint32_t raylib_viewport_width();
uint32_t raylib_viewport_height();
float raylib_get_frame_elapsed_seconds();
