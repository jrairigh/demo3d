#include "mymath.h"

#include <assert.h>
#include <math.h>

#define PI 3.14159265f

const float pi = PI;
const int true = 1;
const int false = 0;
static const float radians_to_degrees_ratio = PI / 180.0f;

float vec2_determinant(const Vec2 p0, const Vec2 p1)
{
    return p0.x * p1.y - p1.x * p0.y;
}

float vec2_magnitude(const Vec2 p)
{
    return sqrtf(p.x * p.x + p.y * p.y);
}

Vec2 vec2_minus_vec2(const Vec2 a, const Vec2 b)
{
    return vec2(a.x - b.x, a.y - b.y);
}

float lerpf(const float a, const float b, const float t)
{
    return (1.0f - t) * a + t * b;
}

MyColor lerp_color(const MyColor a, const MyColor b, const float t)
{
    float red = lerpf(a.red, b.red, t);
    float green = lerpf(a.green, b.green, t);
    float blue = lerpf(a.blue, b.blue, t);
    float alpha = lerpf(a.alpha, b.alpha, t);
    return color((uint8_t)red % 256, (uint8_t)green % 256, (uint8_t)blue % 256, (uint8_t)alpha % 256);
}

bool is_inside_triangle(const Vec2 v, const Vec2 p0, const Vec2 p1, const Vec2 p2)
{
    const Vec2 p0p1 = vec2_minus_vec2(p1, p0);
    const Vec2 p1p2 = vec2_minus_vec2(p2, p1);
    const Vec2 p2p0 = vec2_minus_vec2(p0, p2);
    const Vec2 p0v =  vec2_minus_vec2(v, p0);
    const Vec2 p1v =  vec2_minus_vec2(v, p1);
    const Vec2 p2v =  vec2_minus_vec2(v, p2);

    const float a = vec2_determinant(p0v, p0p1);
    const float b = vec2_determinant(p1v, p1p2);
    const float c = vec2_determinant(p2v, p2p0);

    return a >= 0 && b >= 0 && c >= 0;
}

float is_within_triangle(const Vec2 v, const Vec2 p0, const Vec2 p1, const Vec2 p2)
{
    return -1.0f;
}

bool is_within_canonical_view_volume(const float x, const float y, const float z)
{
    return (-1.0f <= x && x <= 1.0f) &&
        (-1.0f <= y && y <= 1.0f) &&
        (0.0f <= z && z <= 1.0f);
}


Vec3 scalar_x_vec3(const float s, const Vec3 a)
{
    return vec3(a.x * s, a.y * s, a.z * s);
}

Vec4 scalar_x_vec4(const float s, const Vec4 a)
{
    return vec4(a.x * s, a.y * s, a.z * s, a.w * s);
}

Vec3 mat4_x_vec3(const Mat4 m, const Vec3 a)
{
    return vec3(
        (m.v00 * a.x) + (m.v01 * a.y) + (m.v02 * a.z) + m.v03,
        (m.v10 * a.x) + (m.v11 * a.y) + (m.v12 * a.z) + m.v13,
        (m.v20 * a.x) + (m.v21 * a.y) + (m.v22 * a.z) + m.v23
    );
}

Vec4 mat4_x_vec4(const Mat4 m, const Vec4 a)
{
    return vec4(
        (m.v00 * a.x) + (m.v01 * a.y) + (m.v02 * a.z) + (m.v03 * a.w),
        (m.v10 * a.x) + (m.v11 * a.y) + (m.v12 * a.z) + (m.v13 * a.w),
        (m.v20 * a.x) + (m.v21 * a.y) + (m.v22 * a.z) + (m.v23 * a.w),
        (m.v30 * a.x) + (m.v31 * a.y) + (m.v32 * a.z) + (m.v33 * a.w)
    );
}

Mat4 perspective_mat4(const float aspect_ratio, const float fov_degrees, const float near_plane, const float far_plane)
{
    assert(far_plane > near_plane);
    assert(0.0f < fov_degrees && fov_degrees < 180.0f);

    const float z_plane_diff = far_plane - near_plane;
    const float fov_radians = fov_degrees * radians_to_degrees_ratio;
    const float t = tanf(fov_radians * 0.5f);
    const float x = 1.0f / (aspect_ratio * t);
    const float y = 1.0f / t;
    const float z0 = far_plane / z_plane_diff;
    const float z1 = (-far_plane * near_plane) / z_plane_diff;
    return mat4(
        x, 0.0f, 0.0f, 0.0f, 
        0.0f, y, 0.0f, 0.0f,
        0.0f, 0.0f, z0, z1,
        0.0f, 0.0f, 1.0f, 0.0f);
}

Mat4 orthographic_mat4(
    const float left_plane,
    const float right_plane,
    const float bottom_plane,
    const float top_plane,
    const float near_plane,
    const float far_plane)
{
    assert(right_plane > left_plane);
    assert(top_plane > bottom_plane);
    assert(far_plane > near_plane);

    const float sx = 2.0f / (right_plane - left_plane);
    const float sy = 2.0f / (top_plane - bottom_plane);
    const float sz = 1.0f / (far_plane - near_plane);
    const float cx = (left_plane + right_plane) * 0.5f * sx;
    const float cy = (bottom_plane + top_plane) * 0.5f * sy;
    const float cz = near_plane * sz;
    return mat4(
          sx, 0.0f, 0.0f,  -cx,
        0.0f,   sy, 0.0f,  -cy,
        0.0f, 0.0f,   sz,  -cz,
        0.0f, 0.0f, 0.0f, 1.0f);
}

Mat4 identity_mat4()
{
    return mat4(
        1.0f, 0.0f, 0.0f, 0.0f, 
        0.0f, 1.0f, 0.0f, 0.0f, 
        0.0f, 0.0f, 1.0f, 0.0f, 
        0.0f, 0.0f, 0.0f, 1.0f);
}

Mat4 mat4_x_mat4(const Mat4 m, const Mat4 n)
{
    /*
             m          x          n
    | m00 m01 m02 m03 |   | n00 n01 n02 n03 |
    | m10 m11 m12 m13 |   | n10 n11 n12 n13 |
    | m20 m21 m22 m23 |   | n20 n21 n22 n23 |
    | m30 m31 m32 m33 |   | n30 n31 n32 n33 |
    */
    float v00 = (m.v00 * n.v00) + (m.v01 * n.v10) + (m.v02 * n.v20) + (m.v03 * n.v30);
    float v01 = (m.v00 * n.v01) + (m.v01 * n.v11) + (m.v02 * n.v21) + (m.v03 * n.v31);
    float v02 = (m.v00 * n.v02) + (m.v01 * n.v12) + (m.v02 * n.v22) + (m.v03 * n.v32);
    float v03 = (m.v00 * n.v03) + (m.v01 * n.v13) + (m.v02 * n.v23) + (m.v03 * n.v33);

    float v10 = (m.v10 * n.v00) + (m.v11 * n.v10) + (m.v12 * n.v20) + (m.v13 * n.v30);
    float v11 = (m.v10 * n.v01) + (m.v11 * n.v11) + (m.v12 * n.v21) + (m.v13 * n.v31);
    float v12 = (m.v10 * n.v02) + (m.v11 * n.v12) + (m.v12 * n.v22) + (m.v13 * n.v32);
    float v13 = (m.v10 * n.v03) + (m.v11 * n.v13) + (m.v12 * n.v23) + (m.v13 * n.v33);

    float v20 = (m.v20 * n.v00) + (m.v21 * n.v10) + (m.v22 * n.v20) + (m.v23 * n.v30);
    float v21 = (m.v20 * n.v01) + (m.v21 * n.v11) + (m.v22 * n.v21) + (m.v23 * n.v31);
    float v22 = (m.v20 * n.v02) + (m.v21 * n.v12) + (m.v22 * n.v22) + (m.v23 * n.v32);
    float v23 = (m.v20 * n.v03) + (m.v21 * n.v13) + (m.v22 * n.v23) + (m.v23 * n.v33);

    float v30 = (m.v30 * n.v00) + (m.v31 * n.v10) + (m.v32 * n.v20) + (m.v33 * n.v30);
    float v31 = (m.v30 * n.v01) + (m.v31 * n.v11) + (m.v32 * n.v21) + (m.v33 * n.v31);
    float v32 = (m.v30 * n.v02) + (m.v31 * n.v12) + (m.v32 * n.v22) + (m.v33 * n.v32);
    float v33 = (m.v30 * n.v03) + (m.v31 * n.v13) + (m.v32 * n.v23) + (m.v33 * n.v33);

    return mat4(
        v00, v01, v02, v03,
        v10, v11, v12, v13,
        v20, v21, v22, v23,
        v30, v31, v32, v33
    );
}
