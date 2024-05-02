#pragma once

#include "window.h"

void raylib_draw_pixel(const Vec2 normalized_coordinate, const MyColor color);
void raylib_close_window();
void raylib_show(const char* title, const uint32_t viewport_width, const uint32_t viewport_height);
void raylib_on_update(Window* window, const float ts);
void raylib_begin_draw();
void raylib_end_draw();
