#include "window.h"

#include <stdio.h>

const int screen_width = 800;
const int screen_height = 600;

void render(Window* window)
{
    Triangle t[4] = {
        triangle(vec2(-1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(0.0f, 0.0f), color(255,0,0,255)),
        triangle(vec2(-1.0f, 0.0f), vec2(0.0f, 0.0f), vec2(0.0f, -1.0f), color(0,255,0,255)),
        triangle(vec2(0.0f, 1.0f), vec2(1.0f, 0.0f), vec2(0.0f, 0.0f), color(0,0,255,255)),
        triangle(vec2(1.0f, 0.0f), vec2(0.0f, -1.0f), vec2(0.0f, -0.0f), color(255,0,255,255)),
    };
    draw_triangles(window, t, 4);
}

int main()
{
    Window* window = show("3d Demo", screen_width, screen_height);
    update(window, 0.16667f);
    close_window(&window);
    return 0;
}
