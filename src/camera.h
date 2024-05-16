#pragma once

#include "types.h"

typedef struct MyCamera
{
    Vec3 Position;
    Vec3 LookAt;
    Vec3 UpDirection;
    Vec3 StrafeDirection;
    Mat4 MVP;
} MyCamera;

bool is_camera_facing_triangle(const MyCamera camera, const Triangle triangle);

MyCamera perspective_camera(
    const Vec3 position, 
    const Vec3 look_at, 
    const Vec3 up, 
    const float aspect_ratio, 
    const float fov, 
    const float near_plane, 
    const float far_plane);

MyCamera orthographic_camera(
    const Vec3 position,
    const float left_plane, 
    const float right_plane, 
    const float bottom_plane, 
    const float top_plane, 
    const float near_plane, 
    const float far_plane);

void move_camera(MyCamera* camera, const float strafe, const float zoom, const float yaw, const float pitch);
