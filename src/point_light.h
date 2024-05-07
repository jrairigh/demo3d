#pragma once

#include "types.h"

typedef struct PointLight
{
    float Intensity;
    Vec3 Position;
    MyColor Color;
} PointLight;

PointLight point_light(const float intensity, const Vec3 position, const MyColor color);
MyColor color_at_position(const PointLight light, const Vec3 position);
