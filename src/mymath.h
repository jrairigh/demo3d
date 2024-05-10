#pragma once

#include "types.h"

const extern float pi;

// scalar math
float clamp(const float v, const float min_value, const float max_value);

// color math
Vec4 color_as_vec4(const MyColor c);
MyColor color_add_color(const MyColor a, const MyColor b);
float lerpf(const float a, const float b, const float t);
MyColor lerp_color(const MyColor a, const MyColor b, const float t);

// vector math
Vec3 normalized(const Vec3 a);
Vec3 cross_product(const Vec3 a, const Vec3 b);
Vec4 vec3_to_vec4(const Vec3 v, float w);
bool is_inside_triangle(const Vec2 v, const Vec2 p0, const Vec2 p1, const Vec2 p2);
float is_within_triangle(const Vec2 v, const Vec3 p0, const Vec3 p1, const Vec3 p2);
float vec2_determinant(const Vec2 p0, const Vec2 p1);
float vec2_magnitude(const Vec2 p);
float vec3_magnitude(const Vec3 p);
Vec2 vec2_minus_vec2(const Vec2 a, const Vec2 b);
Vec3 vec3_minus_vec3(const Vec3 a, const Vec3 b);
Vec3 vec3_add_vec3(const Vec3 a, const Vec3 b);
Vec3 scalar_x_vec3(const float s, const Vec3 a);
Vec4 scalar_x_vec4(const float s, const Vec4 a);
Vec3 mat3_x_vec3(const Mat3 m, const Vec3 a);
Vec3 mat4_x_vec3(const Mat4 m, const Vec3 a);
Vec4 mat4_x_vec4(const Mat4 m, const Vec4 a);
float vec3_dot_product(const Vec3 a, const Vec3 b);
Vec2 lerp_vec2(const Vec2 start, const Vec2 end, const float t);

// matrix math
Mat3 rotate_y_axis(const float angle);
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
Mat4 translation_mat4(const Vec3 v);
Mat4 mat4_add_mat4(const Mat4 m1, const Mat4 m2);

// other
bool is_within_canonical_view_volume(const float x, const float y, const float z);
