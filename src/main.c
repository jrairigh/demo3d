#include "window.h"
#include "mymath.h"

#include <stdio.h>

const int screen_width = 800;
const int screen_height = 600;

void render(Window* window)
{
    // Draw 4 triangles
    //Triangle t[4] = {
    //  triangle(vec3(-0.2f, 0.2f, 2.0f), vec3(0.2f, 0.2f,  2.0f), vec3(0.0f, 7.0f, 50.0f), color(255,0,0,255)),
    //  triangle(vec3(-0.2f, 0.2f, 2.0f), vec3(0.0f, 7.0f, 50.0f), vec3(-0.2f, -0.2f,  2.0f),  color(0,255,0,255)),
    //  triangle(vec3(0.2f, 0.2f,  2.0f), vec3(0.2f, -0.2f,  2.0f), vec3(0.0f, 7.0f, 50.0f), color(0,0,255,255)),
    //  triangle(vec3(-0.2f, -0.2f,  2.0f), vec3(0.0f, 7.0f, 50.0f), vec3(0.2f, -0.2f, 2.0f),  color(255,0,255,255)),
    //};
    //draw_triangles(window, t, 4);

    // Draw pixels converging to vanishing point
    //for (int i = 1; i < 50; ++i)
    //{
    //    draw_pixel(window, vec3(-0.2f,  0.2f, 10.0f * i), color(255, 0, 0,   255));
    //    draw_pixel(window, vec3(0.2f,   0.2f, 10.0f * i), color(0, 255, 0,   255));
    //    draw_pixel(window, vec3(0.2f,  -0.2f, 10.0f * i), color(0,   0, 255, 255));
    //    draw_pixel(window, vec3(-0.2f, -0.2f, 10.0f * i), color(255, 0, 255, 255));
    //}

    // Draw horizontal and vertical lines converging to vanishing point
    for (int i = 1; i < 50; ++i)
    {
        draw_line(window, vec3(-0.2f, 0.2f, 10.0f * i),  vec3(0.2f, 0.2f, 10.0f * i),   color(255, 0, 0, 255));
        draw_line(window, vec3(0.2f, 0.2f, 10.0f * i),   vec3(0.2f, -0.2f, 10.0f * i),  color(0, 255, 0, 255));
        draw_line(window, vec3(0.2f, -0.2f, 10.0f * i),  vec3(-0.2f, -0.2f, 10.0f * i), color(0, 0, 255, 255));
        draw_line(window, vec3(-0.2f, -0.2f, 10.0f * i), vec3(-0.2f, 0.2f, 10.0f * i),  color(255, 0, 255, 255));
    }

    draw_line(window, vec3(-0.2f, -0.2f, 10.0f), vec3(-0.2f, -0.2f, 500.0f), color(255, 255, 0, 255));
    draw_line(window, vec3(0.2f, -0.2f, 10.0f),  vec3(0.2f, -0.2f, 500.0f),  color(255, 255, 0, 255));
}

int main()
{
    Window* window = show("3d Demo", screen_width, screen_height);
    update(window, 0.16667f);
    close_window(&window);
    return 0;
}
