#include "window.h"

#include <stdio.h>

const int screen_width = 800;
const int screen_height = 600;

void render(Window* window)
{
    Triangle t = (Triangle){ 
        vec2(-0.75f, 0.1f),  // p0
        vec2(0.75f, 0.6f),   // p1
        vec2(0.75f, -0.8f)   // p2
    };
    draw_triangle(window, t, color(0,255,0,255));
}

int main()
{
    Window* window = show("3d Demo", screen_width, screen_height);
    update(window, 0.16667f);
    close_window(&window);
    return 0;
}
