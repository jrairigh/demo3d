#pragma once

#include "types.h"

const extern float pi;

// color math
float lerpf(const float a, const float b, const float t);
MyColor lerp_color(const MyColor a, const MyColor b, const float t);

// vector math
bool is_inside_triangle(const Vec2 v, const Vec2 p0, const Vec2 p1, const Vec2 p2);
float is_within_triangle(const Vec2 v, const Vec2 p0, const Vec2 p1, const Vec2 p2);
float vec2_determinant(const Vec2 p0, const Vec2 p1);
float vec2_magnitude(const Vec2 p);
Vec2 vec2_minus_vec2(const Vec2 a, const Vec2 b);
Vec3 scalar_x_vec3(const float s, const Vec3 a);
Vec4 scalar_x_vec4(const float s, const Vec4 a);
Vec3 mat4_x_vec3(const Mat4 m, const Vec3 a);
Vec4 mat4_x_vec4(const Mat4 m, const Vec4 a);

// matrix math
Mat4 perspective_mat4(const float aspect_ratio, const float fov, const float near_plane, const float far_plane);
Mat4 orthographic_mat4(
    const float left_plane, 
    const float right_plane, 
    const float bottom_plane, 
    const float top_plane, 
    const float near_plane, 
    const float far_plane);
Mat4 identity_mat4();
Mat4 mat4_x_mat4(const Mat4 m1, const Mat4 m2);

// other
bool is_within_canonical_view_volume(const float x, const float y, const float z);
