#pragma once

#include "ray.h"

class Vec3;
class Ray;

class Camera
{
private:

    Point3 m_position;    
    Point3 lower_left_corner;
    Vec3 horizontal;
    Vec3 vertical;
    
public:
    ~Camera() = default;
    Camera() = delete;
    Camera(const Camera& other) = delete;
    Camera& operator=(const Camera& other) = delete;
    Camera(Camera&& other) = delete;
    Camera& operator=(Camera&& other) = delete;

    Camera(Point3 lookfrom, Point3 lookat, Vec3 vup,float vfov, float aspect_ratio);


    Ray get_ray(float u, float v) const;

    
};
