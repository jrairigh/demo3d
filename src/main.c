#include "window.h"

const int screenWidth = 800;
const int screenHeight = 450;

void render(const Window* window)
{
    float x = window->viewport_width / 2.0f;
    float y = window->viewport_height / 2.0f;
    window->draw_pixel(x, y, (MyColor){ 0, 255, 0, 255 });
}

int main()
{
    Window* window = show("3d Demo", screenWidth, screenHeight);
    update(window, 0.16667f);
    close_window(&window);
    return 0;
}
