#include "window.h"
#include "mymath.h"

#include <assert.h>
#include <stdio.h>

const int screen_width = 800;
const int screen_height = 600;

int main()
{
    Window* window = show("3d Demo", screen_width, screen_height);
    update(window);
    close_window(&window);
    return 0;
}

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
    Triangle t[4] = {
        triangle(vec3(-50.0f, 50.0f, 100.0f), vec3(0.0f, 50.0f, 100.0f), vec3(-50.0f, -50.0f, 100.0f), color(250, 0, 0, 255)),
        triangle(vec3(-50.0f, 50.0f, 150.0f), vec3(0.0f, 50.0f, 150.0f), vec3(-50.0f, -50.0f, 150.0f), color(250, 0, 0, 255)),
        triangle(vec3(-50.0f, 50.0f, 250.0f), vec3(0.0f, 50.0f, 250.0f), vec3(-50.0f, -50.0f, 250.0f), color(250, 0, 0, 255)),
        triangle(vec3(-50.0f, 50.0f, 400.0f), vec3(0.0f, 50.0f, 400.0f), vec3(-50.0f, -50.0f, 400.0f), color(250, 0, 0, 255)),
    };

    draw_triangles(window, t, _countof(t));
}

void render_pixels_to_vanishing_point(Window* window)
{
    const float k = g_vec3.x / 10.0f;
    const float s = g_vec3.y / 10.0f;
    const float n = g_vec3.z / 10.0f;
    for (int i = 1; i < n; ++i)
    {
        draw_pixel(window, vec3(-k, k, s * i), color(255, 0, 0, 255));
        draw_pixel(window, vec3(k, k, s * i), color(0, 255, 0, 255));
        draw_pixel(window, vec3(k, -k, s * i), color(0, 0, 255, 255));
        draw_pixel(window, vec3(-k, -k, s * i), color(255, 0, 255, 255));
    }
}

void render_lines_to_vanishing_point(Window* window)
{
    const float k = 100.0f;
    const float s = 20.0f;
    const float n = 100.0f;
    for (int i = 1; i < n; ++i)
    {
        draw_line(window, vec3(-k, k, s * i), vec3(k, k, s * i), color(255, 0, 0, 255));
        draw_line(window, vec3(k, k, s * i), vec3(k, -k, s * i), color(0, 255, 0, 255));
        draw_line(window, vec3(k, -k, s * i), vec3(-k, -k, s * i), color(0, 0, 255, 255));
        draw_line(window, vec3(-k, -k, s * i), vec3(-k, k, s * i), color(255, 0, 255, 255));
    }

    draw_line(window, vec3(-k, -k, s), vec3(-k, -k, s * n), color(255, 255, 0, 255));
    draw_line(window, vec3(k, -k, s), vec3(k, -k, s * n), color(255, 255, 0, 255));
}

void triangle_culling_test(Window* window)
{
    const Vec3 points[] = {
        /*0*/vec3(0.0f, -20.0f, 20.0f),
        /*1*/vec3(20.0f, -20.0f, -20.0f),
        /*2*/vec3(-20.0f, -20.0f, -20.0f),
        /*3*/vec3(0.0f, 20.0f, 0.0f) };
    const int indices[] = {
        //3,2,1,
        //3,1,0,
        3,0,2,
    };

    assert(_countof(indices) % 3 == 0);
    for (int i = 0; i < _countof(indices); i += 3)
    {
        const Vec3 p0 = points[indices[i]];
        const Vec3 p1 = points[indices[i + 1]];
        const Vec3 p2 = points[indices[i + 2]];
        const Vec3 p0p1 = vec3_minus_vec3(p1, p0);
        const Vec3 p1p2 = vec3_minus_vec3(p2, p1);
        const Vec3 normal = normalized(cross_product(p0p1, p1p2));
        const Triangle t = triangle(p0, p1, p2, color(255, 0, 0, 255));
        const Vec3 center_start = get_triangle_center(t);
        const Vec3 center_end = vec3_add_vec3(center_start, scalar_x_vec3(10.0f, normal));
        draw_triangle(window, t);
        draw_line(window, p0, p1, color(255, 255, 0, 255));
        draw_line(window, p1, p2, color(0, 255, 0, 255));
        draw_line(window, p2, p0, color(0, 255, 255, 255));
        draw_line(window, center_start, center_end, color(255, 255, 0, 255));
    }
}

void render(Window* window)
{
    if (window->number_of_lights == 0)
    {
        const PointLight light = point_light(10000.0f, vec3(-10.0f, -10.0f, -10.0f), color(255, 0, 0, 255));
        add_light_source(window, light);
    }

    triangle_culling_test(window);
    //render_4_triangles(window);
    //render_overlapping_triangles(window);
    //draw_cube(window, origin, vec3(20.0f, 20.0f, 20.0f), color(0, 255, 0, 255));
    //draw_grid(window, -10.0f, 10.0f, 10.0f, 10, 10, color(80, 80, 80, 255));
    //Vec3 cam_position = window->camera.Position;
    //Vec3 cam_lookat = window->camera.LookAt;
    //char str1[32] = { 0 }, str2[32] = { 0 };
    //sprintf(str1, "(%0.2f,%0.2f,%0.2f)", cam_position.x, cam_position.y, cam_position.z);
    //sprintf(str2, "(%0.2f,%0.2f,%0.2f)", cam_lookat.x, cam_lookat.y, cam_lookat.z);
    //draw_overlay_text(window, str1, vec2(20.0, 50.0), color(0, 255, 0, 255));
    //draw_overlay_text(window, str2, vec2(20.0, 70.0), color(0, 255, 0, 255));
    //draw_wireframe_box(window, vec3(50.0f, 100.0f, 50.0f), 0.0f, vec3(0.0f, 0.0f, 0.0f), color(255, 255, 0, 255));
    //render_pixels_to_vanishing_point(window);
    //render_lines_to_vanishing_point(window);
    //draw_mesh(window, origin, vec3(30.0f, 30.0f, 30.0f), color(255, 0, 0, 255));
}
