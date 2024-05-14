#include "window.h"
#include "mymath.h"

#include "raylib_impl.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

/*TODO
* 1. Try cross hatching the triangle lines to fill the triangles
* 2. Allow light to affect each pixel of the triangle
* 3. Clipping
* 4. Culling backfaces
* 5. Z buffer
* 6. Camera should face whatever the look at direction is.
* 7. First person shooter camera mode
*/

enum WASD_Keys
{
    W_KEY,
    A_KEY,
    S_KEY,
    D_KEY
};

Vec3 g_vec3 = { 0.0f, 0.0f, 1.0f };

static void post_render(Window* window);

Vec2 get_normalized_coordinate(const Window* window, const Vec2 position)
{
    const float viewport_width = (float)window->get_viewport_width();
    const float viewport_height = (float)window->get_viewport_height();
    const float nx = clamp(lerpf(-1.0f, 1.0f, position.x / viewport_width), -1.0f, 1.0f);
    const float ny = clamp(lerpf(1.0f, -1.0f, position.y / viewport_width), -1.0f, 1.0f);
    return vec2(nx, ny);
}

Vec2 get_position_update_from_input(const Window* window, const float elapsed_seconds)
{
    const bool w_key_down = window->wasd_key_state[W_KEY];
    const bool a_key_down = window->wasd_key_state[A_KEY];
    const bool s_key_down = window->wasd_key_state[S_KEY];
    const bool d_key_down = window->wasd_key_state[D_KEY];
    if (!w_key_down && !a_key_down && !s_key_down && !d_key_down)
        return vec2(0.0f, 0.0f);

    const float speed = window->CameraSpeed;
    float dz = ((int)window->wasd_key_state[W_KEY]) * speed * elapsed_seconds;
    dz += ((int)window->wasd_key_state[S_KEY]) * -1.0f * speed * elapsed_seconds;
    float dx = ((int)window->wasd_key_state[D_KEY]) * speed * elapsed_seconds;
    dx += ((int)window->wasd_key_state[A_KEY]) * -1.0f * speed * elapsed_seconds;

    return vec2(dx * 0.01f, dz);
}

void update_camera_position(Window* window, const float elapsed_seconds)
{
    const Vec2 deltas = get_position_update_from_input(window, elapsed_seconds);
    const float cos_a = window->camera.LookAt.z;
    const float sin_a = window->camera.LookAt.x;
    const float cos_b = cosf(deltas.x);
    const float sin_b = sinf(deltas.x);

    // cos(a + b) = cos(a)cos(b) - sin(a)sin(b)
    const float x = cos_a * cos_b - sin_a * sin_b;
    // sin(a + b) = cos(a)sin(b) + sin(a)cos(b)
    const float z = cos_a * sin_b + sin_a * cos_b;
    window->camera.LookAt = normalized(vec3(z, 0.0f, x));
    window->camera.Position = vec3_add_vec3(window->camera.Position, scalar_x_vec3(deltas.y, window->camera.LookAt));
}

void update_light_position(Window* window, const float elapsed_seconds)
{
    const Vec3 light_position = window->point_lights[0].Position;
    const Vec2 deltas = get_position_update_from_input(window, elapsed_seconds);
    window->point_lights[0].Position = vec3_add_vec3(window->camera.Position, scalar_x_vec3(deltas.y, window->camera.LookAt));
}

Window* create_window_impl()
{
    Window* window = (Window*)malloc(sizeof(Window));
    window->pre_render = raylib_begin_draw;
    window->render = raylib_render;
    window->post_render = raylib_end_draw;
    window->on_update = raylib_on_update;
    window->draw_pixel = raylib_draw_pixel;
    window->draw_line = raylib_draw_line;
    window->draw_light_widget = raylib_light_widget;
    window->draw_overlay_text = raylib_draw_overlay_text;
    window->close_window = raylib_close_window;
    window->show = raylib_show;
    window->get_viewport_width = raylib_viewport_width;
    window->get_viewport_height = raylib_viewport_height;
    window->get_frame_elapsed_seconds = raylib_get_frame_elapsed_seconds;
    window->get_mesh_triangle_points = raylib_get_mesh_triangle_points;
    window->get_mesh_vertex_count = raylib_get_mesh_vertex_count;
    window->get_mesh_triangle_count = raylib_get_mesh_triangle_count;
    return window;
}

static void clear_z_buffer(Window* window)
{
    const uint32_t viewport_width = window->get_viewport_width();
    const uint32_t viewport_height = window->get_viewport_height();

    // TODO need to reallocate z_buffer if viewport changed
    // The canonical view volume z values are between 0 and 1, so choosing an impossible value than will always be overriden by something in view.
    const int n = viewport_width * viewport_height;
    for (int i = 0; i < n; ++i)
        window->z_buffer[i] = 99.0f;
}

Window* show(const char* title, const uint32_t screen_width, const uint32_t screen_height)
{
    Window* window = create_window_impl();
    window->title = title;
    window->screen_width = screen_width;
    window->screen_height = screen_height;
    window->is_open = true;
    window->is_camera_active = true;
    window->show(title, screen_width, screen_height);
    window->number_of_lights = 0;
    window->IsOrthographic = false;
    window->FOV = 90.0f;
    window->NearZ = 0.1f;
    window->FarZ = 100.0f;
    window->CameraSpeed = 100.0f;
    window->MeshId = 6;
    window->Controls = vec4(10.0f, 10.0f, 10.0f, 10.0f);

    const uint32_t viewport_width = window->get_viewport_width();
    const uint32_t viewport_height = window->get_viewport_height();
    window->z_buffer = (float*)malloc(viewport_width * viewport_height * sizeof(float));
    clear_z_buffer(window);

    const Vec3 camera_look_at = z_axis;
    const Vec3 camera_up = y_axis;
    const Vec3 camera_position = vec3(0.0f, 0.0f, -30.0f);
    const float aspect_ratio = (float)viewport_width / (float)viewport_height;
    window->camera = perspective_camera(camera_position, camera_look_at, camera_up, aspect_ratio, window->FOV, window->NearZ, window->FarZ);

    return window;
}

void on_update(Window* window, const float elapsed_seconds)
{
    static bool is_camera_position_restored = 1;
    static Vec3 old_camera_position;
    window->on_update(window, elapsed_seconds);
    clear_z_buffer(window);

    const float viewport_width = (float)window->get_viewport_width();
    const float viewport_height = (float)window->get_viewport_height();
    const float aspect_ratio = viewport_width / viewport_height;
    const float x = fabsf(g_vec3.x);
    const float y = fabsf(g_vec3.y);
    const float z = fabsf(g_vec3.z);

    if (window->is_camera_active && is_camera_position_restored == false)
    {
        is_camera_position_restored = true;
        window->camera.Position = old_camera_position;
    }

    Vec3 camera_position;
    if (window->is_camera_active)
    {
        update_camera_position(window, elapsed_seconds);
        camera_position = window->camera.Position;
        old_camera_position = window->camera.Position;
    }
    else
    {
        is_camera_position_restored = false;
        camera_position = window->point_lights[0].Position;
        update_light_position(window, elapsed_seconds);
    }

    const Vec3 camera_up = y_axis;
    window->camera = window->IsOrthographic
        ? orthographic_camera(camera_position, -x, x, -y, y, -z, z)
        : perspective_camera(camera_position, window->camera.LookAt, camera_up, aspect_ratio, window->FOV, window->NearZ, window->FarZ);
}

void update(Window* window)
{
    while (window->is_open)
    {
        on_update(window, window->get_frame_elapsed_seconds());
        window->pre_render();
        render(window);
        post_render(window);
    }
}

void post_render(Window* window)
{
    //window->render();

    if (window->number_of_lights == 1)
        draw_light_widget(window, window->point_lights[0].Position, 1000.0f, color(255, 255, 0, 255));

    window->post_render(window);
}

void close_window(Window** window_ptr)
{
    Window* w = *window_ptr;
    w->close_window();
    free(w->z_buffer);
    free(w);
    *window_ptr = NULL;
}

void add_light_source(Window* window, const PointLight light)
{
    const uint32_t index = window->number_of_lights;
    window->point_lights[index] = light;
    window->number_of_lights++;
}

void draw_pixel(Window* window, const Vec3 p1, const MyColor color)
{
    const Vec4 p2 = mat4_x_vec4(window->camera.MVP, vec4(p1.x, p1.y, p1.z, 1.0f));
    if (p2.w == 0.0f)
    {
        return;
    }

    const Vec4 position = scalar_x_vec4((1.0f / p2.w), p2);
    const float x = position.x;
    const float y = position.y;
    const float z = position.z;

    const bool inside_view = is_within_canonical_view_volume(x, y, z);
    if (!inside_view)
        return;

    window->draw_pixel(vec2(x, y), color);
}

void draw_line(Window* window, const Vec3 start, const Vec3 end, const MyColor color)
{
    Vec4 start_ = vec3_to_vec4(start, 1.0f);
    Vec4 end_ = vec3_to_vec4(end, 1.0f);
    Vec4 p0 = mat4_x_vec4(window->camera.MVP, start_);
    Vec4 p1 = mat4_x_vec4(window->camera.MVP, end_);

    if (p0.w == 0.0f || p1.w == 0.0f)
        return;

    const Vec4 start_position = scalar_x_vec4((1.0f / p0.w), p0);
    const Vec4 end_position   = scalar_x_vec4((1.0f / p1.w), p1);
    const float sx = start_position.x;
    const float sy = start_position.y;
    const float sz = start_position.z;
    const float ex = end_position.x;
    const float ey = end_position.y;
    const float ez = end_position.z;

    // TODO need to handle lines that are partially clipped
    const bool inside_view = is_within_canonical_view_volume(sx, sy, sz) && is_within_canonical_view_volume(ex, ey, ez);
    if(!inside_view)
        return;

    window->draw_line(vec2(sx, sy), vec2(ex, ey), color);
}

void draw_triangles(Window* window, const Triangles triangle, const uint32_t triangle_count)
{
    for (uint32_t i = 0; i < triangle_count; ++i)
    {
        draw_triangle(window, triangle[i]);
    }
}

void draw_triangle(Window* window, const Triangle triangle)
{
    const MyCamera* camera = &window->camera;

    // transform by view matrix
    const Vec4 p0 = mat4_x_vec4(camera->MVP, vec3_to_vec4(triangle.p0, 1.0f));
    const Vec4 p1 = mat4_x_vec4(camera->MVP, vec3_to_vec4(triangle.p1, 1.0f));
    const Vec4 p2 = mat4_x_vec4(camera->MVP, vec3_to_vec4(triangle.p2, 1.0f));

    if (p0.w * p1.w * p2.w == 0.0f)
        return;

    // scale by w
    const Vec4 p0_scaled = scalar_x_vec4((1.0f / p0.w), p0);
    const Vec4 p1_scaled = scalar_x_vec4((1.0f / p1.w), p1);
    const Vec4 p2_scaled = scalar_x_vec4((1.0f / p2.w), p2);

    // cull
    const bool inside_view = is_within_canonical_view_volume(p0_scaled.x, p0_scaled.y, p0_scaled.z) &&
        is_within_canonical_view_volume(p1_scaled.x, p1_scaled.y, p1_scaled.z) &&
        is_within_canonical_view_volume(p2_scaled.x, p2_scaled.y, p2_scaled.z);
    if (!inside_view)
        return;

    if (!is_camera_facing_triangle(*camera, triangle))
        return;

    // test within triangle and not occluded by something in front
    //const float z = is_within_triangle(device_coordinate,
    //    vec3(p0_.x, p0_.y, p0_.z),
    //    vec3(p1_.x, p1_.y, p1_.z),
    //    vec3(p2_.x, p2_.y, p2_.z));
    //if (z < 0.0f || z > window->z_buffer[i * viewport_width + j])
    //    return;

    //window->z_buffer[i * viewport_width + j] = z;

    const float min_iterations = 200.0f;
    const float min_z = fmaxf(0.1f, fminf(fminf(p0_scaled.z, p1_scaled.z), p2_scaled.z));
    const Vec2 p0_vec2 = vec2(p0_scaled.x, p0_scaled.y);
    const Vec2 p1_vec2 = vec2(p1_scaled.x, p1_scaled.y);
    const Vec2 p2_vec2 = vec2(p2_scaled.x, p2_scaled.y);
    const int iterations = (int)(min_iterations / min_z);
    for (int i = 0; i < iterations; ++i)
    {
        const float t = (float)i / (float)iterations;
        const Vec2 start_1 = lerp_vec2(p0_vec2, p1_vec2, t);
        const Vec2 end_1 = lerp_vec2(p0_vec2, p2_vec2, t);
        const Vec2 start_2 = lerp_vec2(p1_vec2, p0_vec2, t);
        const Vec2 end_2 = lerp_vec2(p1_vec2, p2_vec2, t);

        const MyColor pixel_color = window->number_of_lights == 1
            ? color_at_position(window->point_lights[0], vec3(start_1.x, start_1.y, p0.z))
            : triangle.color;

        window->draw_line(start_1, end_1, pixel_color);
        window->draw_line(start_2, end_2, pixel_color);
    }
}

/*
void draw_triangle(Window* window, const Triangle triangle)
{
    const uint32_t viewport_height = window->get_viewport_height();
    const uint32_t viewport_width = window->get_viewport_width();
    for (uint32_t i = 0; i < viewport_height; ++i)
    {
        for (uint32_t j = 0; j < viewport_width; ++j)
        {
            const Vec2 device_coordinate = get_normalized_coordinate(window, vec2((float)j, (float)i));

            // transform by view matrix
            const Vec4 p0 = mat4_x_vec4(window->camera.MVP, vec3_to_vec4(triangle.p0, 1.0f));
            const Vec4 p1 = mat4_x_vec4(window->camera.MVP, vec3_to_vec4(triangle.p1, 1.0f));
            const Vec4 p2 = mat4_x_vec4(window->camera.MVP, vec3_to_vec4(triangle.p2, 1.0f));

            if (p0.w * p1.w * p2.w == 0.0f)
            {
                continue;
            }

            // scale by w
            const Vec4 p0_ = scalar_x_vec4((1.0f / p0.w), p0);
            const Vec4 p1_ = scalar_x_vec4((1.0f / p1.w), p1);
            const Vec4 p2_ = scalar_x_vec4((1.0f / p2.w), p2);

            // cull
            const bool inside_view = is_within_canonical_view_volume(p0_.x, p0_.y, p0_.z) &&
                is_within_canonical_view_volume(p1_.x, p1_.y, p1_.z) &&
                is_within_canonical_view_volume(p2_.x, p2_.y, p2_.z);
            if (!inside_view)
                continue;

            // test within triangle and not occluded by something in front
            const float z = is_within_triangle(device_coordinate, 
                vec3(p0_.x, p0_.y, p0_.z), 
                vec3(p1_.x, p1_.y, p1_.z), 
                vec3(p2_.x, p2_.y, p2_.z));
            if (z < 0.0f || z > window->z_buffer[i * viewport_width + j])
                continue;

            MyColor pixel_color = triangle.color;
            if (window->number_of_lights > 0)
                pixel_color = color_at_position(window->point_lights[0], vec3(device_coordinate.x, device_coordinate.y, p0.z));

            window->z_buffer[i * viewport_width + j] = z;
            window->draw_pixel(device_coordinate, pixel_color);
        }
    }
}
*/

void draw_overlay_text(Window* window, const char* text, const Vec2 position, const MyColor color)
{
    window->draw_overlay_text(text, position, color);
}

void draw_wireframe_box(Window* window, const Vec3 scale, const float angle, const Vec3 position, const MyColor color)
{
    const float x = scale.x * 0.5f;
    const float y = scale.y * 0.5f;
    const float z = scale.z * 0.5f;

    // box corners
    const Vec3 a = vec3(-x, y, -z);
    const Vec3 b = vec3(x, y, -z);
    const Vec3 c = vec3(x, -y, -z);
    const Vec3 d = vec3(-x, -y, -z);
    const Vec3 e = vec3(-x, y, z);
    const Vec3 f = vec3(x, y, z);
    const Vec3 g = vec3(x, -y, z);
    const Vec3 h = vec3(-x, -y, z);

    // rotate corners
    const Mat3 rotate_y_matrix = rotate_y_axis(angle);
    const Vec3 a_ = vec3_add_vec3(mat3_x_vec3(rotate_y_matrix, a), position);
    const Vec3 b_ = vec3_add_vec3(mat3_x_vec3(rotate_y_matrix, b), position);
    const Vec3 c_ = vec3_add_vec3(mat3_x_vec3(rotate_y_matrix, c), position);
    const Vec3 d_ = vec3_add_vec3(mat3_x_vec3(rotate_y_matrix, d), position);
    const Vec3 e_ = vec3_add_vec3(mat3_x_vec3(rotate_y_matrix, e), position);
    const Vec3 f_ = vec3_add_vec3(mat3_x_vec3(rotate_y_matrix, f), position);
    const Vec3 g_ = vec3_add_vec3(mat3_x_vec3(rotate_y_matrix, g), position);
    const Vec3 h_ = vec3_add_vec3(mat3_x_vec3(rotate_y_matrix, h), position);

    // front face
    draw_line(window, a_, b_, color);
    draw_line(window, b_, c_, color);
    draw_line(window, c_, d_, color);
    draw_line(window, d_, a_, color);
    draw_line(window, a_, c_, color);

    // back face
    draw_line(window, e_, f_, color);
    draw_line(window, f_, g_, color);
    draw_line(window, g_, h_, color);
    draw_line(window, h_, e_, color);
    draw_line(window, h_, f_, color);

    // top face
    draw_line(window, a_, e_, color);
    draw_line(window, b_, f_, color);
    draw_line(window, b_, e_, color);

    // bottom face
    draw_line(window, d_, h_, color);
    draw_line(window, c_, g_, color);
    draw_line(window, d_, g_, color);

    // side faces
    draw_line(window, a_, h_, color);
    draw_line(window, c_, f_, color);
}

void draw_grid(Window* window, const float y, const float cell_width, const float cell_height, const uint32_t rows, const uint32_t columns, const MyColor color)
{
    const float grid_width = cell_width * (float)columns;
    const float grid_height = cell_height * (float)rows;
    const float half_grid_width = grid_width * 0.5f;
    const float half_grid_height = grid_height * 0.5f;

    for (uint32_t i = 0; i <= columns; ++i)
    {
        const float z = i * cell_height - half_grid_height;
        const Vec3 start = vec3(-half_grid_width, y, z);
        const Vec3 end = vec3(half_grid_height, y, z);
        draw_line(window, start, end, color);
    }

    for (uint32_t i = 0; i <= rows; ++i)
    {
        const float x = i * cell_width - half_grid_width;
        const Vec3 start = vec3(x, y, -half_grid_height);
        const Vec3 end = vec3(x, y, half_grid_height);
        draw_line(window, start, end, color);
    }

    draw_line(window, vec3(0.0f, -10.0f, 0.0f), vec3(0.0f, 10.0f, 0.0f), color);
}

void draw_cube(Window* window, const Vec3 position, const Vec3 scale, const MyColor cube_color)
{
    const Vec3 half_scale = scalar_x_vec3(0.5f, scale);

    // Naming convention based on camera facing down positive z axis and front is the part camera is facing
    const Triangle back_face_tri_1 = triangle(
        vec3(-half_scale.x, -half_scale.y, -half_scale.z),
        vec3(-half_scale.x, half_scale.y, -half_scale.z),
        vec3(half_scale.x, half_scale.y, -half_scale.z),
        cube_color
    );
    const Triangle back_face_tri_2 = triangle(
        vec3(half_scale.x, half_scale.y, -half_scale.z),
        vec3(half_scale.x, -half_scale.y, -half_scale.z),
        vec3(-half_scale.x, -half_scale.y, -half_scale.z),
        cube_color
    );
    const Triangle front_face_tri_1 = triangle(
        vec3(-half_scale.x, -half_scale.y, half_scale.z),
        vec3(-half_scale.x, half_scale.y, half_scale.z),
        vec3(half_scale.x, half_scale.y, half_scale.z),
        cube_color
    );
    const Triangle front_face_tri_2 = triangle(
        vec3(half_scale.x, half_scale.y, half_scale.z),
        vec3(half_scale.x, -half_scale.y, half_scale.z),
        vec3(-half_scale.x, -half_scale.y, half_scale.z),
        cube_color
    );
    const Triangle bottom_face_tri_1 = triangle(
        vec3(-half_scale.x, -half_scale.y, -half_scale.z),
        vec3(-half_scale.x, -half_scale.y, half_scale.z),
        vec3(half_scale.x, -half_scale.y, half_scale.z),
        cube_color
    );
    const Triangle bottom_face_tri_2 = triangle(
        vec3(half_scale.x, -half_scale.y, half_scale.z),
        vec3(half_scale.x, -half_scale.y, -half_scale.z),
        vec3(-half_scale.x, -half_scale.y, -half_scale.z),
        cube_color
    );
    const Triangle top_face_tri_1 = triangle(
        vec3(-half_scale.x, half_scale.y, -half_scale.z),
        vec3(-half_scale.x, half_scale.y, half_scale.z),
        vec3(half_scale.x, half_scale.y, half_scale.z),
        cube_color
    );
    const Triangle top_face_tri_2 = triangle(
        vec3(half_scale.x, half_scale.y, half_scale.z),
        vec3(half_scale.x, half_scale.y, -half_scale.z),
        vec3(-half_scale.x, half_scale.y, -half_scale.z),
        cube_color
    );
    const Triangle left_face_tri_1 = triangle(
        vec3(-half_scale.x, -half_scale.y, half_scale.z),
        vec3(-half_scale.x, half_scale.y, half_scale.z),
        vec3(-half_scale.x, half_scale.y, -half_scale.z),
        cube_color
    );
    const Triangle left_face_tri_2 = triangle(
        vec3(-half_scale.x, half_scale.y, -half_scale.z),
        vec3(-half_scale.x, -half_scale.y, -half_scale.z),
        vec3(-half_scale.x, -half_scale.y, half_scale.z),
        cube_color
    );
    const Triangle right_face_tri_1 = triangle(
        vec3(half_scale.x, -half_scale.y, -half_scale.z),
        vec3(half_scale.x, half_scale.y, -half_scale.z),
        vec3(half_scale.x, half_scale.y, half_scale.z),
        cube_color
    );
    const Triangle right_face_tri_2 = triangle(
        vec3(half_scale.x, half_scale.y, half_scale.z),
        vec3(half_scale.x, -half_scale.y, half_scale.z),
        vec3(half_scale.x, -half_scale.y, -half_scale.z),
        cube_color
    );

    const Triangle triangle[12] = {
        front_face_tri_1, front_face_tri_2,
        back_face_tri_1, back_face_tri_2,
        bottom_face_tri_1, bottom_face_tri_2,
        top_face_tri_1, top_face_tri_2,
        left_face_tri_1, left_face_tri_2,
        right_face_tri_1, right_face_tri_2,
    };

    draw_triangles(window, triangle, _countof(triangle));
}

void draw_light_widget(Window* window, const Vec3 center, const float radius, const MyColor color)
{
    const Vec4 center_transformed = mat4_x_vec4(window->camera.MVP, vec3_to_vec4(center, 1.0f));
    const Vec4 normalized_center = scalar_x_vec4((1.0f / center_transformed.w), center_transformed);
    const float min_distance = 0.1f;
    const Vec3 camera_to_center = vec3_minus_vec3(center, window->camera.Position);

    // don't draw if camera isn't facing it
    if (vec3_dot_product(window->camera.LookAt, normalized(camera_to_center)) < 0.0f)
        return;

    const float distance = fmaxf(min_distance, vec3_magnitude(camera_to_center));
    window->draw_light_widget(vec2(normalized_center.x, normalized_center.y), radius / distance, color);
}

void draw_mesh(Window* window, const Vec3 position, const Vec3 scale, const MyColor mesh_color)
{
    const int mesh_id_unused = window->MeshId;
    const uint32_t triangle_count = window->get_mesh_triangle_count(mesh_id_unused);
    const uint32_t vertex_count = window->get_mesh_vertex_count(mesh_id_unused);
    const float* points = window->get_mesh_triangle_points(mesh_id_unused);

    for (uint32_t i = 0; i < triangle_count; ++i)
    {
        const int offset = i * 9;
        const float* index = points + offset;
        const Vec3 p0 = vec3(index[0], index[1], index[2]);
        const Vec3 p1 = vec3(index[3], index[4], index[5]);
        const Vec3 p2 = vec3(index[6], index[7], index[8]);
        const Triangle tri = triangle(p0, p1, p2, mesh_color);
        draw_triangle(window, tri);
    }

    // mesh info
    char buffer[256] = { 0 };
    sprintf(buffer, "%d Triangles,  %d Vertices", triangle_count, vertex_count);
    draw_overlay_text(window, buffer, vec2(10.0f, 10.0f), color(0, 255, 0, 255));
}
