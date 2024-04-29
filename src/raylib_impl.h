#pragma once

#include "window.h"

void raylib_draw_pixel(float x, float y, MyColor color);
void raylib_close_window();
void raylib_show(const char* title, uint32_t viewport_width, uint32_t viewport_height);
void raylib_on_update(Window* window, float ts);
