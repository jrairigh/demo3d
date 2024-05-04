#include "types.h"

Vec2 vec2(const float x, const float y)
{
    return (Vec2) { x, y };
}

Vec3 vec3(const float x, const float y, const float z)
{
    return (Vec3) { x, y, z };
}

Vec4 vec4(const float x, const float y, const float z, const float w)
{
    return (Vec4) { x, y, z, w };
}

MyColor color(const uint8_t red, const uint8_t green, const uint8_t blue, const uint8_t alpha)
{
    return (MyColor) { red, green, blue, alpha };
}

Triangle triangle(const Vec3 p0, const Vec3 p1, const Vec3 p2, const MyColor color)
{
    Triangle t;
    t.p0 = p0;
    t.p1 = p1;
    t.p2 = p2;
    t.color = color;
    return t;
}

Mat4 mat4(
    const float v00, const float v01, const float v02, const float v03,
    const float v10, const float v11, const float v12, const float v13,
    const float v20, const float v21, const float v22, const float v23,
    const float v30, const float v31, const float v32, const float v33)
{
    return (Mat4) {
        v00, v01, v02, v03,
        v10, v11, v12, v13,
        v20, v21, v22, v23,
        v30, v31, v32, v33
    };
}
