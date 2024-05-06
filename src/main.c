#include "window.h"
#include "mymath.h"

#include <stdio.h>

const int screen_width = 800;
const int screen_height = 600;

void render_4_triangles(Window* window)
{
    Triangle t[4] = {
        triangle(vec3(-0.2f, 0.2f, 2.0f), vec3(0.2f, 0.2f,  2.0f), vec3(0.0f, 7.0f, 50.0f), color(255,0,0,255)),
        triangle(vec3(-0.2f, 0.2f, 2.0f), vec3(0.0f, 7.0f, 50.0f), vec3(-0.2f, -0.2f,  2.0f),  color(0,255,0,255)),
        triangle(vec3(0.2f, 0.2f,  2.0f), vec3(0.2f, -0.2f,  2.0f), vec3(0.0f, 7.0f, 50.0f), color(0,0,255,255)),
        triangle(vec3(-0.2f, -0.2f,  2.0f), vec3(0.0f, 7.0f, 50.0f), vec3(0.2f, -0.2f, 2.0f),  color(255,0,255,255)),
    };

    draw_triangles(window, t, _countof(t));
}

void render_overlapping_triangles(Window* window)
{
    //const float k = fabsf(g_vec3.x) / 500.0f;
    //const float s = g_vec3.y * 0.1f;
    //const float n = g_vec3.z * 0.1f;
    Triangle t[4] = {
        triangle(vec3(-50.0f, 50.0f, 100.0f), vec3(0.0f, 50.0f, 100.0f), vec3(-50.0f, -50.0f, 100.0f), color(255, 0, 0, 255)),
        triangle(vec3(-50.0f, 50.0f, 150.0f), vec3(0.0f, 50.0f, 150.0f), vec3(-50.0f, -50.0f, 150.0f), color(190, 0, 0, 255)),
        triangle(vec3(-50.0f, 50.0f, 250.0f), vec3(0.0f, 50.0f, 250.0f), vec3(-50.0f, -50.0f, 250.0f), color(150, 0, 0, 255)),
        triangle(vec3(-50.0f, 50.0f, 400.0f), vec3(0.0f, 50.0f, 400.0f), vec3(-50.0f, -50.0f, 400.0f), color(100, 0, 0, 255)),
    };

    draw_triangles(window, t, _countof(t));
}

void render_cube(Window* window)
{
    const float k = g_vec3.x / 10.0f;
    const float s = g_vec3.y / 10.0f;
    const float n = g_vec3.z / 10.0f;

    // Draw a cube
    // front face
    draw_line(window, vec3(-k, k, n), vec3(k, k, n), color(255, 255, 0, 255));
    draw_line(window, vec3(k, k, n), vec3(k, -k, n), color(255, 255, 0, 255));
    draw_line(window, vec3(k, -k, n), vec3(-k, -k, n), color(255, 255, 0, 255));
    draw_line(window, vec3(-k, -k, n), vec3(-k, k, n), color(255, 255, 0, 255));
    draw_line(window, vec3(-k, k, n), vec3(k, -k, n), color(255, 255, 0, 255));
    // back face
    draw_line(window, vec3(-k, k, n + s), vec3(k, k, n + s), color(255, 255, 0, 255));
    draw_line(window, vec3(k, k, n + s), vec3(k, -k, n + s), color(255, 255, 0, 255));
    draw_line(window, vec3(k, -k, n + s), vec3(-k, -k, n + s), color(255, 255, 0, 255));
    draw_line(window, vec3(-k, -k, n + s), vec3(-k, k, n + s), color(255, 255, 0, 255));
    draw_line(window, vec3(-k, -k, n + s), vec3(k, k, n + s), color(255, 255, 0, 255));
    // top face
    draw_line(window, vec3(-k, k, n), vec3(-k, k, n + s), color(255, 255, 0, 255));
    draw_line(window, vec3(k, k, n), vec3(k, k, n + s), color(255, 255, 0, 255));
    draw_line(window, vec3(k, k, n), vec3(-k, k, n + s), color(255, 255, 0, 255));
    // bottom face
    draw_line(window, vec3(-k, -k, n), vec3(-k, -k, n + s), color(255, 255, 0, 255));
    draw_line(window, vec3(k, -k, n), vec3(k, -k, n + s), color(255, 255, 0, 255));
    draw_line(window, vec3(-k, -k, n), vec3(k, -k, n + s), color(255, 255, 0, 255));
    // side faces
    draw_line(window, vec3(-k, k, n), vec3(-k, -k, n + s), color(255, 255, 0, 255));
    draw_line(window, vec3(k, -k, n), vec3(k, k, n + s), color(255, 255, 0, 255));
}

void render_pixels_to_vanishing_point(Window* window)
{
    const float k = g_vec3.x / 10.0f;
    const float s = g_vec3.y / 10.0f;
    const float n = g_vec3.z / 10.0f;
    for (int i = 1; i < n; ++i)
    {
        draw_pixel(window, vec3(-k,  k, s * i), color(255, 0, 0,   255));
        draw_pixel(window, vec3(k,   k, s * i), color(0, 255, 0,   255));
        draw_pixel(window, vec3(k,  -k, s * i), color(0,   0, 255, 255));
        draw_pixel(window, vec3(-k, -k, s * i), color(255, 0, 255, 255));
    }
}

void render_lines_to_vanishing_point(Window* window)
{
    const float k = g_vec3.x / 10.0f;
    const float s = g_vec3.y / 10.0f;
    const float n = g_vec3.z / 10.0f;
    for (int i = 1; i < n; ++i)
    {
        draw_line(window, vec3(-k, k,  s * i), vec3(k, k,   s * i), color(255, 0, 0, 255));
        draw_line(window, vec3(k, k,   s * i), vec3(k, -k,  s * i), color(0, 255, 0, 255));
        draw_line(window, vec3(k, -k,  s * i), vec3(-k, -k, s * i), color(0, 0, 255, 255));
        draw_line(window, vec3(-k, -k, s * i), vec3(-k, k,  s * i), color(255, 0, 255, 255));
    }
    
    draw_line(window, vec3(-k, -k, s), vec3(-k, -k, s * n), color(255, 255, 0, 255));
    draw_line(window, vec3(k, -k,  s), vec3(k, -k,  s * n), color(255, 255, 0, 255));
}

void render(Window* window)
{
    //render_4_triangles(window);
    render_overlapping_triangles(window);
    //render_cube(window);
    //render_pixels_to_vanishing_point(window);
    //render_lines_to_vanishing_point(window);

    //const float k = g_vec3.x / 10.0f;
    //const float s = g_vec3.y / 10.0f;
    //const float n = g_vec3.z / 10.0f;
    //const Mat4 cube = orthographic_mat4(1.0f, 100.0f, 1.0f, 99.9f, 100.0f, 999.9f);
    //const Vec3 in = vec3(g_fov, g_near_z, g_far_z);
    //const Vec3 out = mat4_x_vec3(cube, in);
    //char buffer[256] = { 0 };
    //sprintf(&buffer, "<%.1f, %.1f, %.1f>", in.x, in.y, in.z);
    //draw_overlay_text(window, buffer, vec2(100.0f, 100.0f), color(0, 255, 0, 100));
    //
    //char buffer2[256] = { 0 };
    //sprintf(&buffer2, "<%.1f, %.1f, %.1f>", out.x, out.y, out.z);
    //draw_overlay_text(window, buffer2, vec2(100.0f, 130.0f), color(0, 255, 0, 100));

    //const Mat4 mp = perspective_mat4(4.0f / 3.0f, 60.0f, 1.0f, 999.9f);
    //const Vec4 in = vec4(g_fov, g_near_z, g_far_z, 1.0f);
    //const Vec4 out = mat4_x_vec4(mp, in);
    //char buffer[256] = { 0 };
    //sprintf(&buffer, "<%.1f, %.1f, %.1f>", in.x, in.y, in.z);
    //draw_overlay_text(window, buffer, vec2(100.0f, 100.0f), color(0, 255, 0, 100));
    //
    //char buffer2[256] = { 0 };
    //sprintf(&buffer2, "<%.1f, %.1f, %.1f, %.1f>", out.x, out.y, out.z, out.w);
    //draw_overlay_text(window, buffer2, vec2(100.0f, 130.0f), color(0, 255, 0, 100));
}

int main()
{
    Window* window = show("3d Demo", screen_width, screen_height);
    update(window);
    close_window(&window);
    return 0;
}
