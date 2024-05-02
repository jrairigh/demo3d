#include "mymath.h"

#include <math.h>

const float pi = 3.14159265f;

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

int is_inside_triangle(const Vec2 v, const Vec2 p0, const Vec2 p1, const Vec2 p2)
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

Vec3 mat4_x_vec3(const Mat4 m, const Vec3 a)
{
    return vec3(
        (m.v00 * a.x) + (m.v01 * a.y) + (m.v02 * a.z) + m.v03,
        (m.v10 * a.x) + (m.v11 * a.y) + (m.v12 * a.z) + m.v13,
        (m.v20 * a.x) + (m.v21 * a.y) + (m.v22 * a.z) + m.v23
    );
}

Mat4 perspective_matrix(const float aspect_ratio, const float fov_degrees, const float near_plane, const float far_plane)
{
    float fov_radians = fov_degrees * (pi / 180.0f);
    float t = tanf(fov_radians / 2.0f);
    float x = 1.0f / (aspect_ratio * t);
    float y = 1.0f / t;
    float z0 = far_plane / (far_plane - near_plane);
    float z1 = (-far_plane * near_plane) / (far_plane - near_plane);
    return matrix4(
        x, 0.0f, 0.0f, 0.0f, 
        0.0f, y, 0.0f, 0.0f,
        0.0f, 0.0f, z0, z1,
        0.0f, 0.0f, 1.0f, 0.0f);
}

Mat4 identity_mat4()
{
    return matrix4(
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

    return matrix4(
        v00, v01, v02, v03,
        v10, v11, v12, v13,
        v20, v21, v22, v23,
        v30, v31, v32, v33
    );
}
