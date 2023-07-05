#ifndef CAMERA_H
#define CAMERA_H

#include <numbers>
#include "ray.h"

class Vec3;
class Ray;

class Camera
{
private:

    Point3 m_position;    
    Vec3 m_horizontal; //width of the viewport
    Vec3 m_vertical; //height of viewport
    Point3 lower_left_corner; //coordinates of bottom left of viewport
    
public:

    Camera(const Point3& lookfrom, const Point3& lookat, const Vec3& vup, float vfov, float aspect_ratio) 
    {
        const auto theta = float{vfov}; // * std::numbers::pi_v<float> / 180.f; //In Degrees
        const auto h{tanf(theta/2)};
        const auto viewport_height{2.f * h};
        const auto viewport_width {aspect_ratio * viewport_height};

        const auto w = Norm3{lookfrom - lookat}; // backwards (facing reverse of forwards)
        const auto u = Norm3{Cross(vup, w) }; // left
        const auto v = Norm3{Cross(w, u)}; //up

        m_position = lookfrom;
        m_horizontal = viewport_width * u;
        m_vertical = viewport_height * v;
        lower_left_corner = m_position - m_horizontal/2 - m_vertical/2 - w;
    }

    /// @brief 
    /// @param u 
    /// @param v 
    /// @return 
    [[nodiscard]] constexpr Ray GetRay(float u, float v) const {
        return Ray(m_position, lower_left_corner + u*m_horizontal + v*m_vertical - m_position);
    }

};

#endif