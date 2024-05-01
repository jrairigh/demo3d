#pragma once

#include <stdint.h>

typedef struct _tagColor
{
    uint8_t red, green, blue, alpha;
} MyColor;

typedef struct _tagVec2
{
    float x, y;
} Vec2;

typedef struct _tagVec3
{
    float x, y, z;
} Vec3;

typedef struct _tagMatrix
{
    /*    0  1  2  3
      0 | v  v  v  v |
      1 | v  v  v  v |
      2 | v  v  v  v |
      3 | v  v  v  v |
    */
    float v00, v01, v02, v03,
        v10, v11, v12, v13,
        v20, v21, v22, v23,
        v30, v31, v32, v33;
} Mat4;

typedef struct _tagTriangle
{
    Vec3 p0, p1, p2;
    MyColor color;
} Triangle;

typedef Triangle* Triangles;

// Type constructors
Triangle triangle(const Vec3 p0, const Vec3 p1, const Vec3 p2, const MyColor color);
MyColor color(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha);
Vec2 vec2(const float x, const float y);
Vec3 vec3(const float x, const float y, const float z);
Mat4 matrix4(
    const float v00, const float v01, const float v02, const float v03, 
    const float v10, const float v11, const float v12, const float v13, 
    const float v20, const float v21, const float v22, const float v23, 
    const float v30, const float v31, const float v32, const float v33);
