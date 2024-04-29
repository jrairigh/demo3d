#include "window.h"

#include "raylib_impl.h"

Window* create_window_impl()
{
    Window* window = (Window*)malloc(sizeof(Window));
    window->on_update = raylib_on_update;
    window->draw_pixel = raylib_draw_pixel;
    window->close_window = raylib_close_window;
    window->show = raylib_show;
    return window;
}

Window* show(const char* title, uint32_t viewport_width, uint32_t viewport_height)
{
    Window* window = create_window_impl();
    window->title = title;
    window->viewport_width = viewport_width;
    window->viewport_height = viewport_height;
    window->is_open = 1;
    window->show(title, viewport_width, viewport_height);
    return window;
}

void update(Window* window, float ts)
{
    while (window->is_open > 0)
    {
        window->on_update(window, ts);
        render(window);
    }
}

void close_window(Window** window_ptr)
{
    Window* w = *window_ptr;
    w->close_window();
    free(w);
    w = NULL;
}
