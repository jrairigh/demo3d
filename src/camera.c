#include "camera.h"

#include "mymath.h"

#include <assert.h>
#include <math.h>

bool is_camera_facing_triangle(const MyCamera camera, const Triangle triangle)
{
    const Vec3 center = get_triangle_center(triangle);
    const Vec3 p0p1 = vec3_minus_vec3(triangle.p1, triangle.p0);
    const Vec3 p1p2 = vec3_minus_vec3(triangle.p2, triangle.p1);
    const Vec3 triangle_normal = cross_product(p0p1, p1p2);
    const Vec3 triangle_direction_from_camera = vec3_minus_vec3(center, camera.Position);
    const float facing_triangle_normal_amount = vec3_dot_product(triangle_normal, camera.LookAt);
    const float in_front_of_amount = vec3_dot_product(triangle_direction_from_camera, camera.LookAt);
    return facing_triangle_normal_amount < 0.0f && in_front_of_amount > 0.0f;
}

MyCamera perspective_camera(
    const Vec3 position,
    const Vec3 look_at,
    const Vec3 up,
    const float aspect_ratio,
    const float fov,
    const float near_plane,
    const float far_plane)
{
    // assume look at direction is normalized
    assert(vec3_magnitude(look_at) - 1.0f < 0.0001f);

    const float x = look_at.x;
    const float z = look_at.z;
    const Mat4 translation = translation_mat4(scalar_x_vec3(-1.0f, position));
    const Mat4 rotation = mat4(
        z,    0.0f, -x,   0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        x,    0.0f, z,    0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    const Mat4 perspective_matrix = perspective_mat4(aspect_ratio, fov, near_plane, far_plane);

    const Mat4 translate_and_rotate_matrix = mat4_x_mat4(rotation, translation);
    MyCamera camera;
    camera.Position = position;
    camera.LookAt = look_at;
    camera.UpDirection = up;
    camera.StrafeDirection = cross_product(look_at, camera.UpDirection);
    camera.MVP = mat4_x_mat4(perspective_matrix, translate_and_rotate_matrix);
    return camera;
}

MyCamera orthographic_camera(
    const Vec3 position,
    const float left_plane,
    const float right_plane,
    const float bottom_plane,
    const float top_plane,
    const float near_plane,
    const float far_plane)
{
    MyCamera camera;
    camera.LookAt = z_axis;
    camera.UpDirection = y_axis;
    camera.StrafeDirection = x_axis;
    camera.Position = position;
    camera.MVP = orthographic_mat4(left_plane, right_plane, bottom_plane, top_plane, near_plane, far_plane);
    camera.MVP.v03 = -position.x;
    camera.MVP.v13 = -position.y;
    camera.MVP.v23 = -position.z;
    return camera;
}

void move_camera(MyCamera* camera, const float strafe, const float zoom, const float yaw, const float pitch)
{
    const float cos_a = camera->LookAt.z;
    const float sin_a = camera->LookAt.x;
    const float cos_b = cosf(yaw);
    const float sin_b = sinf(yaw);

    // cos(a + b) = cos(a)cos(b) - sin(a)sin(b)
    const float z = cos_a * cos_b - sin_a * sin_b;
    // sin(a + b) = cos(a)sin(b) + sin(a)cos(b)
    const float x = cos_a * sin_b + sin_a * cos_b;
    const Vec3 new_look_at = normalized(vec3(x, 0.0f, z));
    const Vec3 new_strafe_direction = cross_product(new_look_at, camera->UpDirection);

    const Vec3 forward_amount = scalar_x_vec3(zoom, new_look_at);
    const Vec3 strafe_amount = scalar_x_vec3(strafe, new_strafe_direction);
    const Vec3 move_vector = vec3_add_vec3(forward_amount, strafe_amount);
    const Vec3 new_position = vec3_add_vec3(camera->Position, move_vector);

    camera->LookAt = new_look_at;
    camera->StrafeDirection = new_strafe_direction;
    camera->Position = new_position;
}
