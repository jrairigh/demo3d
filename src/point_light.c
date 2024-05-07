#include "point_light.h"

#include "mymath.h"

PointLight point_light(const float intensity, const Vec3 position, const MyColor color)
{
    return (PointLight) {
        intensity,
        position,
        color
    };
}

MyColor color_at_position(const PointLight light, const Vec3 position)
{
    const float src_red = light.Color.red / 255.0f;
    const float src_green = light.Color.green / 255.0f;
    const float src_blue = light.Color.blue / 255.0f;
    const float src_alpha = light.Color.alpha / 255.0f;
    const Vec4 light_color = vec4(src_red, src_green, src_blue, src_alpha);
    const float distance_to_light = vec3_magnitude(vec3_minus_vec3(position, light.Position));
    const float intensity_at_point = light.Intensity / (distance_to_light * distance_to_light);
    const Vec4 attenuated_color = scalar_x_vec4(intensity_at_point, light_color);
    const uint8_t dest_red = (uint8_t)lerpf(0.0f, 255.0f, attenuated_color.x);
    const uint8_t dest_green = (uint8_t)lerpf(0.0f, 255.0f, attenuated_color.y);
    const uint8_t dest_blue = (uint8_t)lerpf(0.0f, 255.0f, attenuated_color.z);
    const uint8_t dest_alpha = (uint8_t)lerpf(0.0f, 255.0f, attenuated_color.w);
    return color(dest_red, dest_green, dest_blue, dest_alpha);
}
