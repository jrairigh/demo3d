#include "camera.h"

#include "mymath.h"

#include <assert.h>
#include <math.h>

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

    //const Vec3 n = cross_product(look_at, up);
    const float dot_product = vec3_dot_product(look_at, z_axis);
    const float angle = acosf(dot_product);
    const Mat4 model_view_matrix = mat4(
        cosf(angle), 0.0f, -sinf(angle), -position.x,
        0.0f,        1.0f,        0.0f,  -position.y,
        sinf(angle), 0.0f, cosf(angle),  -position.z,
        0.0f,        0.0f,        0.0f,         1.0f
    );

    MyCamera camera;
    camera.Position = position;
    camera.LookAt = look_at;
    camera.MVP = mat4_x_mat4(
        perspective_mat4(aspect_ratio, fov, near_plane, far_plane), model_view_matrix);
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
    camera.Position = position;
    camera.MVP = orthographic_mat4(left_plane, right_plane, bottom_plane, top_plane, near_plane, far_plane);
    camera.MVP.v03 = -position.x;
    camera.MVP.v13 = -position.y;
    camera.MVP.v23 = -position.z;
    return camera;
}
